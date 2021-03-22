#include <iostream>
#include <chrono>
#include <csignal>

#include <Client.h>

using namespace std;
using namespace chrono;

class Client;

static Client client[7];

class Multi_Ch_HV
{
 public:
  Multi_Ch_HV(const string& a_path, const bool& a_verbosity = false);
  ~Multi_Ch_HV();

  void Run();
  static void Signal_Handler(int signal);
  static void Update();

  //Client client[7];

 protected:
  string path;
  bool verbosity;
  
  float vmon[7];
  float imon[7];  

  void Init();
  void Monitor();
};
