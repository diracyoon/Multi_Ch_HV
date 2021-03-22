#include <iostream>
#include <cstdlib>
#include <string>

#include <QApplication>

#include "MainWidget.h"

using namespace std;

int main(int argc, char *argv[])
{
 if(argc!=1&&argc!=2)
    {
      cout << "Wrong number of argc." << endl;
      cout << "argv[1]: mode i.e. NORMAL or DEBUG" << endl;
      cout << "argv[2]: verbosity ie true or false" << endl;
      exit(1);
    }

  string mode;
  if(argc==1) mode = "NORMAL";
  else if(argc==2) mode = argv[1];

  bool verbosity = true;

  cout << "Mode = " << mode << ", verbosity = " << verbosity << endl;

  QApplication a(argc, argv);
  MainWidget w(mode.c_str(), verbosity);
  w.show();

  return a.exec();
}
