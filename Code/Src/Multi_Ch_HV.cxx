#include "Multi_Ch_HV.h"

//////////

Multi_Ch_HV::Multi_Ch_HV(const string& a_path, const bool& a_verbosity) 
: path(a_path), verbosity(a_verbosity)
{
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
  cout << "interrupt" << endl;
  
  if(signal==SIGINT)
  {
    Update();
  }
  return;
}//void Multi_Ch_HV::Signal_Handler(int signal)

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

      //client[i].Request_HV_Cotrol_Set();
    }

  return;
}//void Multi_Ch_HV::Init()

//////////

void Multi_Ch_HV::Monitor()
{
  for(int i=0; i<7; i++)
  {
    vmon[i] = client[i].Request_HV_Control_Get("VMon");
    imon[i] = client[i].Request_HV_Control_Get("IMonH");

    cout << "Ch = " << i << ", V_mon = " << vmon[i] << ", I_mon = " << imon[i] << endl;
  }

  return;
}

//////////

void Multi_Ch_HV::Update()
{
  cout << "update" << endl;

  for(int i=0; i<7; i++)
  {
    float vset = 100;
    client[i].Request_HV_Control_Set("VSet", vset);
  }

  return;
}//void Multi_Ch_HV::Update()

//////////