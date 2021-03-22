#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <iostream>
#include <csignal>

#include <QWidget>
#include <QString>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

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

  void closeEvent(QCloseEvent *event);
  QString Get_Last_Sentence();
  void Start_Server();

 public slots:
  void Read_Process_Output(QProcess *process);
  void Start_Multi_Ch_HV();
  void Update_VSet();
};

#endif // MAINWIDGET_H
