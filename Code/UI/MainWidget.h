#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>

#include <QWidget>
#include <QString>
#include <QProcess>
#include <QTextEdit>
#include <QTextBrowser>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

using namespace std;

class MainWidget : public QWidget
{
  Q_OBJECT

public:
  MainWidget(const QString& a_mode, const bool& a_verbosity, QWidget *parent = nullptr);
  ~MainWidget();

private:
  Ui::MainWidget *ui;

  QString mode;
  bool verbosity;

  QString path;
  QString tty;

  QProcess* tty_finder;
  QProcess* server;
  QProcess* multi_ch_hv;

  float vset_prev[7];
  float vset[7];

  QTextEdit* TE_vset[7];
  QTextBrowser* TB_vmon[7];
  QTextBrowser* TB_imon[7];

  void closeEvent(QCloseEvent *event);
  QString Get_Last_Sentence();
  void Make_Config_File();
  void Start_Server();
  void Read_Config_Data();

 public slots:
  void Read_Process_Output(QProcess *process);
  void Start_Multi_Ch_HV();
  void Update_VSet();
};

#endif // MAINWIDGET_H
