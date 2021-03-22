#include "MainWidget.h"
#include "ui_MainWidget.h"

MainWidget::MainWidget(const QString& a_mode, const bool& a_verbosity, QWidget *parent) : mode(a_mode), verbosity(a_verbosity), QWidget(parent), ui(new Ui::MainWidget)
{
  ui->setupUi(this);

  server = NULL;
  multi_ch_hv = NULL;

  connect(ui->button_start, SIGNAL(clicked(bool)), SLOT(Start_Multi_Ch_HV()));
  connect(ui->button_update, SIGNAL(clicked(bool)), SLOT(Update_VSet()));

  path = getenv("MULTI_CH_HV_SW_PATH");

  if(mode=="DEBUG")
   {
     ui->text->append("DEBUG mode set");

     tty = "test_port";
   }
  else
    {
      ui->text->append("NORMAL mode set");

      ui->text->append("Finding device tty");

      QProcess* tty_finder = new QProcess;
      connect(tty_finder, &QProcess::readyRead, [=](){ this->Read_Process_Output(tty_finder); });

      QString target = path + "/Macro/TTY_Finder.sh";

      tty_finder->start(target, QStringList());
      tty_finder->waitForFinished();

      delete tty_finder;

      QString sentence = Get_Last_Sentence();

      if(sentence.contains("Error")) return;
      else tty = sentence;
    }

   ui->text->append("TTY = " + tty);

   Start_Server();
}//MainWidget::MainWidget(const QString& a_mode, const bool& a_verbosity, QWidget *parent)

//////////

MainWidget::~MainWidget()
{
  delete ui;
}//MainWidget::~MainWidget()

//////////

void MainWidget::closeEvent(QCloseEvent *event)
{
  if(server!=NULL) server->close();
  if(multi_ch_hv!=NULL) multi_ch_hv->close();

  return;
}//void MainWidget::closeEvent(QCloseEvent *event)

//////////

QString MainWidget::Get_Last_Sentence()
{
  QTextCursor text_cursor = ui->text->textCursor();
  text_cursor.movePosition(QTextCursor::End);
  text_cursor.select(QTextCursor::LineUnderCursor);
  QString last_sentence = text_cursor.selectedText();

  return last_sentence;
}//QString MainWidget::Get_Last_Sentence()

//////////

void MainWidget::Start_Server()
{
  ui->text->append("Starting Server...");

  //remove fifo opened previously
  ui->text->append("Remove FIFO opened previously");

  QString target = "rm " + path + "/FIFOs/FIFO";
  QProcess rm_fifo;
  rm_fifo.start("/bin/bash", QStringList() << "-c" << target);
  rm_fifo.waitForFinished();

  //start server
  target = path + "/Bin/Server";

  QStringList arg;
  arg << tty << mode;

  server = new QProcess;
  connect(server, &QProcess::readyRead, [=](){ this->Read_Process_Output(server); });

  server->start(target, arg);
  server->waitForStarted();

  ui->text->append("Server started.");

  return;
}//void MainWidget::Start_Server()

//////////

void MainWidget::Read_Process_Output(QProcess *process)
{
  QString str = process->readAll().trimmed();
  ui->text->append(str);

  return;
}//void MainWidget::Read_Process_Output(QProcess *process)

//////////

void MainWidget::Start_Multi_Ch_HV()
{
  if(multi_ch_hv==NULL)
    {
      ui->text->append("Starting Multi_Ch_HV...");

      //remove FIFO opened previously
      QString target = "rm " + path + "/FIFOs/FIFO?";
      QProcess rm_fifo;
      rm_fifo.start("/bin/bash", QStringList() << "-c" << target);
      rm_fifo.waitForFinished();

      //start Multi_Ch_HV_Controller
      target = path + "/Bin/Multi_Ch_HV";

      QStringList arg;
      arg << path;

      multi_ch_hv = new QProcess;
      connect(multi_ch_hv, &QProcess::readyRead, [=](){ this->Read_Process_Output(multi_ch_hv); });

      multi_ch_hv->start(target, arg);
      multi_ch_hv->waitForStarted();

      ui->text->append("Multi_Ch_HV started.");
    }
  else ui->text->append("Multi_Ch_HV is running already.");

  return;
}//void MainWidget::Start_Multi_Ch_HV()

//////////

void MainWidget::Update_VSet()
{
  if(multi_ch_hv!=NULL)
    {
      ui->text->append("Updating V_set");
      int pid = multi_ch_hv->pid();

      //ui->text->append(multi_ch_hv->pid());
      kill(pid, 2);
    }
  else ui->text->append("Multi_Ch_HV is not running.");


  return;
}//void MainWidget::Update_VSet()

//////////
