#include <iostream>
#include <chrono>
#include <csignal>
#include <ctime>
#include <string>

#include <Client.h>

using namespace std;
using namespace chrono;

class Client;

static string path;
static Client client[7];
static float vset[7];
static float vset_old[7];

class Multi_Ch_HV
{
 public:
  Multi_Ch_HV(const string& a_path, const bool& a_verbosity = false);
  ~Multi_Ch_HV();

  void Run();
  static void Signal_Handler(int signal);
  static void Update();
  static void Read_Config_Data(const string& type="Init");

 protected:
  bool verbosity;
  
  float vmon[7];
  float imon[7];  

  int date_yesterday;
  string name_yesterday;
  int min_old;
  bool chk_monitor_out;
  
  ofstream monitor_out;

  void Init();
  void Monitor();

};
