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

  //start server and communication to HV modules
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

   //
   TE_vset[0] = ui->TE_DG_vset;
   TE_vset[1] = ui->TE_GEM1_vset;
   TE_vset[2] = ui->TE_TR1G_vset;
   TE_vset[3] = ui->TE_GEM2_vset;
   TE_vset[4] = ui->TE_TR2G_vset;
   TE_vset[5] = ui->TE_GEM3_vset;
   TE_vset[6] = ui->TE_IG_vset;

   TB_vmon[0] = ui->TB_DG_vmon;
   TB_vmon[1] = ui->TB_GEM1_vmon;
   TB_vmon[2] = ui->TB_TR1G_vmon;
   TB_vmon[3] = ui->TB_GEM2_vmon;
   TB_vmon[4] = ui->TB_TR2G_vmon;
   TB_vmon[5] = ui->TB_GEM3_vmon;
   TB_vmon[6] = ui->TB_IG_vmon;

   TB_imon[0] = ui->TB_DG_imon;
   TB_imon[1] = ui->TB_GEM1_imon;
   TB_imon[2] = ui->TB_TR1G_imon;
   TB_imon[3] = ui->TB_GEM2_imon;
   TB_imon[4] = ui->TB_TR2G_imon;
   TB_imon[5] = ui->TB_GEM3_imon;
   TB_imon[6] = ui->TB_IG_imon;

   Read_Config_Data();

  for(int i=0; i<7; i++)
  {
    QString text = QString::number(vset[i]);
    TE_vset[i]->setText(text);
  }

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

void MainWidget::Make_Config_File()
{
  QString config_file = path + "/Config/Update.config";

  ofstream fout_config;
  fout_config.open(config_file.toStdString());

  for(int i=0; i<7; i++) fout_config << vset[i] << endl;

  fout_config.close();

  return;
}//void MainWidget::Make_Config_File()

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

void MainWidget::Read_Config_Data()
{
  ifstream fin_config;
  QString config_file = path + "/Config/Multi_Ch_HV.config";

  fin_config.open(config_file.toStdString());
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
}//void MainWidget::Read_Config_Data()

//////////

void MainWidget::Read_Process_Output(QProcess* process)
{
  QString str = process->readAll().trimmed();

  if(!str.contains("Monitor")) ui->text->append(str);
  else
    {
      istringstream iss(str.toStdString());

      string temp;

      iss >> temp;
      iss >> temp;

      int ch;
      iss >> ch;

      iss >> temp;

      float vmon;
      iss >> vmon;

      iss >> temp;

      float imon;
      iss >> imon;

      TB_vmon[ch]->setText(QString::number(vmon));
      TB_imon[ch]->setText(QString::number(imon));
    }

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

      for(int i=0; i<7; i++)
        {
          vset_prev[i] = vset[i];

          QString temp = TE_vset[i]->toPlainText();
          vset[i] = temp.toDouble();
         }

      QString message = "Please confirm your inputs are all right.<br>";
      message += "DG  : " + QString::number(vset[0]) + "<br>";
      message += "GEM1: " + QString::number(vset[1]) + "<br>";
      message += "TR1G: " + QString::number(vset[2]) + "<br>";
      message += "GEM2: " + QString::number(vset[3]) + "<br>";
      message += "TR2G: " + QString::number(vset[4]) + "<br>";
      message += "GEM3: " + QString::number(vset[5]) + "<br>";
      message += "IG:   " + QString::number(vset[6]) + "<br>";

      QMessageBox::StandardButton button_proceed = QMessageBox::question(this, "Confirm", "<font size = 5>"+message+"<\font>", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
      if(button_proceed==QMessageBox::Yes)
        {
          Make_Config_File();

          int pid = multi_ch_hv->pid();
          kill(pid, SIGINT);
        }
      else
        {
          ui->text->append("Update was cancelled.");

          for(int i=0; i<7; i++)
            {
              vset[i] = vset_prev[i];
              TE_vset[i]->setText(QString::number(vset[i]));
            }
        }
    }
  else ui->text->append("Multi_Ch_HV is not running.");

  return;
}//void MainWidget::Update_VSet()

//////////
