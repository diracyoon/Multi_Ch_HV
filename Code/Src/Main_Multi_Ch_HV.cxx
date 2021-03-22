#include <iostream>
#include <cstdlib>

#include "Multi_Ch_HV.h"

using namespace std;

int main(int argc, char* argv[])
{
  char* path = getenv("MULTI_CH_HV_SW_PATH");
  if(path==NULL)
    {
      cout << "Enviromental variables aren't set. Source Env.sh first." << endl;

      return -1;
    }

  Multi_Ch_HV multi_ch_hv(path, true);
  multi_ch_hv.Run();  

  return 0;
}
