#include "Multi_Ch_HV.h"

//////////

Multi_Ch_HV::Multi_Ch_HV(const string& a_path, const bool& a_verbosity) 
: verbosity(a_verbosity), date_yesterday(-1), min_old(-1), chk_monitor_out(true)
{
  path = a_path;

  for(int i=0; i<7; i++) vset_old[i] = -1;

  Read_Config_Data();
  Init();
}//Multi_Ch_HV::Multi_Ch_HV()

//////////

Multi_Ch_HV::~Multi_Ch_HV()
{
  for(int i=0; i<7; i++)
  {
    client[i].Request_HV_Control_Set("Pw", 0);
  }
}//Multi_Ch_HV::~Multi_Ch_HV()

//////////

void Multi_Ch_HV::Run()
{
  signal(SIGINT, Signal_Handler);

  while(1)
  {
    Monitor();

    this_thread::sleep_for(chrono::seconds(1));
  }

  return;
}//void Multi_Ch_HV::Run()

//////////

void Multi_Ch_HV::Signal_Handler(int signal)
{
  if(signal==SIGINT) Update();
  
  return;
}//void Multi_Ch_HV::Signal_Handler(int signal)

//////////

void Multi_Ch_HV::Update()
{
  Read_Config_Data("Update");

  for(int i=0; i<7; i++)
  {
    cout << "Ch= " << i << " Vset= " << vset[i] << endl;
    client[i].Request_HV_Control_Set("VSet", vset[i]);
  }

  return;
}//void Multi_Ch_HV::Update()

//////////

void Multi_Ch_HV::Read_Config_Data(const string& type)
{
  ifstream fin_config;
  string config_file = path + "/Config/";
  if(type.compare("Init")==0) config_file += "Multi_Ch_HV.config"; 
  else if(type.compare("Update")==0) config_file += "Update.config";

  fin_config.open(config_file);
  if(!fin_config.is_open()) throw "class Multi_Ch_HV: Can not find the config file.";

  string buf;
  int index = 0;
  while(!fin_config.eof())
  {
    getline(fin_config, buf);
    if(buf.empty()) break;

    istringstream iss(buf);

    iss >> vset[index];

    index++;
  }

  return;
}//void Multi_Ch_HV::Read_Config_Data()

//////////

void Multi_Ch_HV::Init()
{
  cout << "Initializing Multi_Ch_HV." << endl;
  
  for(int i=0; i<7; i++)
    {
      cout << "Ch = " << i << endl;
      client[i].Set_Channel(i);
      client[i].Set_Path(path);
      client[i].Set_Verbosity(verbosity);
      client[i].Initialization();
      client[i].Connect_To_Server();

      client[i].Request_HV_Control_Set("VSet", vset[i]);
    }

  return;
}//void Multi_Ch_HV::Init()

//////////

void Multi_Ch_HV::Monitor()
{
  /*getting time info.*/
  time_t current_time = time(NULL);
  struct tm* time_tm = localtime(&current_time);
  
  int date_today = time_tm->tm_mday;

  if(date_today!=date_yesterday)
    {
      if(monitor_out.is_open()) monitor_out.close(); 

      //open file for today
      int year = time_tm->tm_year + 1900;
      int month = time_tm->tm_mon + 1;
      int date = time_tm->tm_mday;

      string name_today = to_string(year);
      name_today += "_";
      name_today += string(2 - to_string(month).length(), '0') + to_string(month);
      name_today += "_";
      name_today += string(2 - to_string(date).length(), '0') + to_string(date);

      name_today += ".log";
      name_today = path + "/Logs/" + name_today;

      monitor_out.open(name_today, std::ofstream::app);

      name_yesterday = name_today;
      date_yesterday = date_today;
    }//if(date_today!=date_yesterday)

  //when vsets are updated, print these to log
  for(int i=0; i<7; i++)
  {
    if(vset_old[i]!=vset[i])
    {
      chk_monitor_out = true;
      break;
    }
  }

  int min = time_tm->tm_min;
  
  //print log at interval of 1 min
  if(min_old!=min) chk_monitor_out = true;

  if(chk_monitor_out) monitor_out << time_tm->tm_hour << ":" << min << ":" << time_tm->tm_sec << " ";
  for(int i=0; i<7; i++)
  {
    vmon[i] = client[i].Request_HV_Control_Get("VMon");
    imon[i] = client[i].Request_HV_Control_Get("IMonH");

    cout << "Monitor: Ch= " << i << " V_mon= " << vmon[i] << " I_mon= " << imon[i] << endl;

    if(chk_monitor_out) monitor_out << vset[i] << " " << vmon[i] << " " << imon[i] << " ";
  }
  if(chk_monitor_out) monitor_out << endl;

  if(chk_monitor_out) 
  {
    chk_monitor_out = false;
    min_old = min;
    for(int i=0; i<7; i++) vset_old[i] = vset[i];
  }

  return;
}//void Multi_Ch_HV::Monitor()

//////////