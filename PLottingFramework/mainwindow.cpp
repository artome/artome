#include "mainwindow.h"
#include "dbapi.h"
#include "server.h"
#include "ui_mainwindow.h"
#include "xydata.h"
#include <QPixmap>
#include <cmath>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  serverheader = "<html><head/><body><p><span style=\" font-weight:600; "
                 "font-style:italic;\">CloudMonitoring</span></p><p><span "
                 "style=\" font-weight:600; font-style:italic;\">connection "
                 "dialog</span></p></body></html>";
  databaseconnection = "plotframework";
  connect(ui->actionfirsttest, SIGNAL(triggered()), this, SLOT(firsttest()));
  connect(ui->actionTestbanana, SIGNAL(triggered()), this, SLOT(testebanana()));
  connect(ui->actiontestbanana_xy, SIGNAL(triggered()), this, SLOT(testebananaxy()));
  connect(ui->actiontestehisto1, SIGNAL(triggered()), this, SLOT(testehisto1()));
  connect(ui->actiontestehisto2, SIGNAL(triggered()), this, SLOT(testehisto2()));
  colors.clear();
  colors << "0000FF"
         << "FF0000"
         << "008000"
         << "FF00FF"
         << "800080"
         << "00FFFF"
         << "808000"
         << "800000"
         << "800080"
         << "69bdd2";
  ConnectDatabase();
 }

MainWindow::~MainWindow() { delete ui; }

void MainWindow::ConnectDatabase() {
  Server dialog;

  int result;

  dialog.connectionname = databaseconnection;
  QString lixo;
  lixo = serverheader;
  lixo = lixo.replace("CloudMonitoring", "Main Database");
  dialog.labeltext(lixo);
  result = dialog.exec();

  if (result) {
    setWindowTitle("Plotframework " + dialog.cdatabase);
  }
}

void MainWindow::firsttest() {
  long long timei, timef;
  Plotting plot;
  plotproprieties p;
  connect(&plot, SIGNAL(logmessages(QString)), this,
          SLOT(LogMessages(QString)));

 // timei = 1573463400001;
  timei =1573464404000;
  //  timef = 1573487100000;
  timef = 1573497100000;
 // LogMessages(plot.getstartruns(timei,timef,databaseconnection));
  QVector<xydata *> xy;
  std::vector<double> vec;
  std::vector<long long> time;
  QRect geo = this->ui->graphicsView->geometry();
  int xlen = geo.width();
  int ylen = geo.height();
  ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  dbapi table;
  table.connectionName = databaseconnection;
  connect(&table, SIGNAL(logmessages(QString)), this,
          SLOT(LogMessages(QString)));
  connect(&table, SIGNAL(logmessagesR(QString)), this,
          SLOT(LogMessagesR(QString)));
  QStringList tnames, cnames;
  tnames.clear();
  cnames.clear();
  tnames << "SMPS_MPSS_data"
         << "SMPS_MPSS_data"
         << "SMPS_MPSS_data"
         << "SMPS_MPSS_data";
  cnames << "Temp"
         << "Presion"
         << "RH"
         << "d52_979000";

  for (int i = 0; i < tnames.count(); i++) {
    xy << new xydata;

    if (i == tnames.count() - 1)
      xy << new xydata;

    xy[i]->p.Ymax = 10;
    xy[i]->p.Ymin = 0;
    xy[i]->p.maxfreelimits = true;
    xy[i]->p.minfreelimits = true;
    xy[i]->p.Color = colors.at(i);
    xy[i]->p.yaxisLabel = cnames.at(i);
    vec.clear();
    time.clear();
    table.fech2columns_timeanddouble(tnames.at(i), cnames.at(i), timei, timef,
                                     &time, &vec);
    unsigned long size = time.size();

    for (unsigned long j = 0; j < size; j++) {
      xy[i]->x.push_back(time.at(j) / 1000.);
      xy[i]->y.push_back(vec.at(j));

      if (i == tnames.count() - 1) {
        xy[i + 1]->x.push_back(time.at(j) / 1000.);
        xy[i + 1]->y.push_back(vec.at(j));
      }
    }
  }

  xy << new xydata;

  for (long long i = timei; i < timef; i = i + 10000) {
    xy[5]->x.push_back(i / 1000.);
    xy[5]->y.push_back(5 * (i / 1000.) * (i / 1000.));
  }
  p.withoutxaxis=true;
  xy[2]->p.rightaxis = true;
  xy[3]->p.rightaxis = true;
  xy[2]->p.leftaxis = false;
  xy[3]->p.leftaxis = false;
  // xy[3]->p.smooth=true;
  xy[3]->p.smoothalpha = 0.2;
  xy[3]->p.smoothpoints = 2;
  //  xy[3]->p.ylog=true;
  xy[3]->p.Ymin = 10.;
  xy[3]->p.Ylmin = std::log(xy[3]->p.Ymin);
  //  xy[3]->p.minfreelimits=false;
  //  xy[3]->p.linetype="Vbars";
  //  xy[3]->p.linetype="markers";
  xy[3]->p.derivative = true;
  xy[3]->p.devnpoints = 10;
  xy[4]->p.maxfreelimits = true;
  xy[4]->p.minfreelimits = true;
  xy[4]->p.Color = colors.at(4);
  xy[4]->p.yaxisLabel = cnames.at(3);
  xy[5]->p.maxfreelimits = true;
  xy[5]->p.minfreelimits = true;
  xy[5]->p.Color = colors.at(5);
  xy[5]->p.derivative = true;
  xy[5]->p.devnpoints = 10;

  xy[5]->p.yaxisLabel = "derivada de uma parabola";
  //xy[5]->p.rightaxis=true;
  p.multiaxissep = .10;
  p.labelsep = 1.0;
  p.yvmin = 0.0;
  p.xvmin=0.10;
  p.Plotname = "testing proprieties";
  plot.Plot_page(xlen, ylen, "memqt", "", 255, 255, 255, 1.);
  plot.SetInicialColormap();
  p.Multixais=2;
  if(p.Multixais==0)plot.Singleaxis(timei / 1000, timef / 1000, &xy, &p);
  else if(p.Multixais==1)plot.Multiaxis(timei / 1000, timef / 1000, &xy, &p);
  else if(p.Multixais==2)plot.Twoaxis(timei / 1000, timef / 1000, &xy, &p);
  QStringList runs=plot.getstartruns(timei,timef,databaseconnection).split(";");
  plot.putrunlabels(timei / 1000, timef / 1000,&p,runs); //put labels at borders
 // plot.putrunlabels(1,timei / 1000, timef / 1000,&p,runs);
  plot.deleteStream();

  for (int i = 0; i < tnames.count(); i++) {
    delete xy[i];
  }

  delete xy[tnames.count()];
  delete xy[tnames.count() + 1];
  xy.clear();

  plot.image->rgbSwapped().save("lixo100.png");
  QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
  scene.clear();
  scene.addPixmap(*renderer);
  this->ui->graphicsView->setScene(&scene);
  delete renderer;
  plot.deleteImage();
}

void MainWindow::testebanana() {
    long long timei = 1573463400001;
    //  timef = 1573487100000;
    long long timef = 1573497100000;
    long long timeii = 1573473400001;
    long long timeff = 1573487100000;
    Plotting plot;
    plotproprieties p;
    connect(&plot, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));

  QString tname = "SMPS_MPSS_data";
  QStringList cnames;

  cnames.clear();
  cnames << "d8_732000"
         << "d8_980000"
         << "d9_498000"
         << "d10_046000"
         << "d10_626000"
         << "d11_240000"
         << "d11_890000"
         << "d12_578000"
         << "d13_307000"
         << "d14_079000"
         << "d14_896000"
         << "d15_762000"
         << "d16_680000"
         << "d17_652000"
         << "d18_682000"
         << "d19_774000"
         << "d20_932000"
         << "d22_159000"
         << "d23_461000"
         << "d24_841000"
         << "d26_306000"
         << "d27_861000"
         << "d29_512000"
         << "d31_264000"
         << "d33_126000"
         << "d35_104000"
         << "d37_207000"
         << "d39_442000"
         << "d41_821000"
         << "d44_352000"
         << "d47_047000"
         << "d49_918000"
         << "d52_979000"
         << "d56_243000"
         << "d59_726000"
         << "d63_446000"
         << "d67_421000"
         << "d71_672000"
         << "d76_299000"
         << "d81_098000"
         << "d86_326000"
         << "d91_937000"
         << "d97_968000"
         << "d104_455000"
         << "d111_443000"
         << "d118_979000"
         << "d127_118000"
         << "d135_920000"
         << "d145_454000"
         << "d155_798000"
         << "d167_036000"
         << "d179_268000"
         << "d192_605000"
         << "d207_171000"
         << "d223_108000"
         << "d240_576000"
         << "d259_754000"
         << "d280_845000"
         << "d304_077000"
         << "d329_706000"
         << "d358_019000"
         << "d389_336000"
         << "d424_014000"
         << "d462_451000"
         << "d505_091000"
         << "d552_426000"
         << "d605_006000"
         << "d663_443000"
         << "d728_419000"
         << "d800_699000"
         << "d839_861000";
/*
      QVector<double> diameters;
      diameters.clear();
      diameters << 8.732000 << 8.980000 << 9.498000 << 10.046000 << 10.626000
                << 11.240000 << 11.890000 << 12.578000 << 13.307000 << 14.079000
                << 14.896000 << 15.762000 << 16.680000 << 17.652000 << 18.682000
                << 19.774000 << 20.932000 << 22.159000 << 23.461000 << 24.841000
                << 26.306000 << 27.861000 << 29.512000 << 31.264000 << 33.126000
                << 35.104000 << 37.207000 << 39.442000 << 41.821000 << 44.352000
                << 47.047000 << 49.918000 << 52.979000 << 56.243000 << 59.726000
                << 63.446000 << 67.421000 << 71.672000 << 76.299000 << 81.098000
                << 86.326000 << 91.937000 << 97.968000 << 104.455000 <<111.443000
                << 118.979000 << 127.118000 << 135.920000 << 145.454000
                << 155.798000 << 167.036000 << 179.268000 << 192.605000
                << 207.171000 << 223.108000 << 240.576000 << 259.754000
                << 280.845000 << 304.077000 << 329.706000 << 358.019000
                << 389.336000 << 424.014000 << 462.451000 << 505.091000
                << 552.426000 << 605.006000 << 663.443000 << 728.419000
                << 800.699000 << 839.861000;

 */
 // std::vector<double> diameters;

//  diameters.clear();
    xyzdata xyz;
  xyz.y.push_back(8.732000);
  xyz.y.push_back(8.980000);
  xyz.y.push_back(9.498000);
  xyz.y.push_back(10.046000);
  xyz.y.push_back(10.626000);
  xyz.y.push_back(11.240000);
  xyz.y.push_back(11.890000);
  xyz.y.push_back(12.578000);
  xyz.y.push_back(13.307000);
  xyz.y.push_back(14.079000);
  xyz.y.push_back(14.896000);
  xyz.y.push_back(15.762000);
  xyz.y.push_back(16.680000);
  xyz.y.push_back(17.652000);
  xyz.y.push_back(18.682000);
  xyz.y.push_back(19.774000);
  xyz.y.push_back(20.932000);
  xyz.y.push_back(22.159000);
  xyz.y.push_back(23.461000);
  xyz.y.push_back(24.841000);
  xyz.y.push_back(26.306000);
  xyz.y.push_back(27.861000);
  xyz.y.push_back(29.512000);
  xyz.y.push_back(31.264000);
  xyz.y.push_back(33.126000);
  xyz.y.push_back(35.104000);
  xyz.y.push_back(37.207000);
  xyz.y.push_back(39.442000);
  xyz.y.push_back(41.821000);
  xyz.y.push_back(44.352000);
  xyz.y.push_back(47.047000);
  xyz.y.push_back(49.918000);
  xyz.y.push_back(52.979000);
  xyz.y.push_back(56.243000);
  xyz.y.push_back(59.726000);
  xyz.y.push_back(63.446000);
  xyz.y.push_back(67.421000);
  xyz.y.push_back(71.672000);
  xyz.y.push_back(76.299000);
  xyz.y.push_back(81.098000);
  xyz.y.push_back(86.326000);
  xyz.y.push_back(91.937000);
  xyz.y.push_back(97.968000);
  xyz.y.push_back(104.455000);
  xyz.y.push_back(111.443000);
  xyz.y.push_back(118.979000);
  xyz.y.push_back(127.118000);
  xyz.y.push_back(135.920000);
  xyz.y.push_back(145.454000);
  xyz.y.push_back(155.798000);
  xyz.y.push_back(167.036000);
  xyz.y.push_back(179.268000);
  xyz.y.push_back(192.605000);
  xyz.y.push_back(207.171000);
  xyz.y.push_back(223.108000);
  xyz.y.push_back(240.576000);
  xyz.y.push_back(259.754000);
  xyz.y.push_back(280.845000);
  xyz.y.push_back(304.077000);
  xyz.y.push_back(329.706000);
  xyz.y.push_back(358.019000);
  xyz.y.push_back(389.336000);
  xyz.y.push_back(424.014000);
  xyz.y.push_back(462.451000);
  xyz.y.push_back(505.091000);
  xyz.y.push_back(552.426000);
  xyz.y.push_back(605.006000);
  xyz.y.push_back(663.443000);
  xyz.y.push_back(728.419000);
  xyz.y.push_back(800.699000);
  xyz.y.push_back(839.861000);

  dbapi table;
  table.connectionName = databaseconnection;
  connect(&table, SIGNAL(logmessages(QString)), this,
          SLOT(LogMessages(QString)));
  connect(&table, SIGNAL(logmessagesR(QString)), this,
          SLOT(LogMessagesR(QString)));
 std::vector<long long> time;
 QVector<std::vector<double> *> c;
 for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
  table.fechvariouscolumns_timeanddoubles(tname, cnames, timei,timef, &time,&c);
  xyz.z.resize(cnames.count());
  for (int j=0; j<(int)time.size(); j++){
      xyz.x.push_back(time.at(j)/1000.);
      for (int kk = 0; kk < cnames.count(); kk++) {
          xyz.z[kk].push_back(c.at(kk)->at(j)); //factor de escala aqui.
      }
  }
  for(int i=0;i<c.count();i++){
      delete c.at(i);
  }

  QRect geo = this->ui->graphicsView->geometry();
  int xlen = geo.width();
  int ylen = geo.height();
  ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//  p.multiaxissep = .10;
//  p.labelsep = 1.0;
  p.yvmin = 0.0;
  p.withoutxaxis=true;
  p.Plotname = "testing proprieties";
 // xyz.p.colorscale="nocolorscale";
  xyz.p.colorscale="insidecolorscale";
  xyz.p.surfacetype="countour";
  xyz.p.ylog=true;
  xyz.p.zlog=true;
  xyz.p.zlogmin=1;
  plot.Plot_page(xlen, ylen, "memqt", "", 255, 255, 255, 1.);
  plot.SetInicialColormap(); //must be called after Plot_page that inicializes pl

  plot.SurfacePlot(timeii / 1000., timeff / 1000., &xyz, &p);
  plot.deleteStream();


  plot.image->rgbSwapped().save("lixo100.png");
  QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
  scene.clear();
  scene.addPixmap(*renderer);
  this->ui->graphicsView->setScene(&scene);
  delete renderer;
  plot.deleteImage();
}
void MainWindow::testebananaxy(){
    long long timei = 1573463400001;
    //  timef = 1573487100000;
    long long timef = 1573497100000;
    long long timeii = 1573473400001;
    long long timeff = 1573487100000;
    Plotting plot;
    plotproprieties p;
    connect(&plot, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));

    QString tname = "SMPS_MPSS_data";
    QStringList cnames;

    cnames.clear();
    cnames << "d8_732000"
           << "d8_980000"
           << "d9_498000"
           << "d10_046000"
           << "d10_626000"
           << "d11_240000"
           << "d11_890000"
           << "d12_578000"
           << "d13_307000"
           << "d14_079000"
           << "d14_896000"
           << "d15_762000"
           << "d16_680000"
           << "d17_652000"
           << "d18_682000"
           << "d19_774000"
           << "d20_932000"
           << "d22_159000"
           << "d23_461000"
           << "d24_841000"
           << "d26_306000"
           << "d27_861000"
           << "d29_512000"
           << "d31_264000"
           << "d33_126000"
           << "d35_104000"
           << "d37_207000"
           << "d39_442000"
           << "d41_821000"
           << "d44_352000"
           << "d47_047000"
           << "d49_918000"
           << "d52_979000"
           << "d56_243000"
           << "d59_726000"
           << "d63_446000"
           << "d67_421000"
           << "d71_672000"
           << "d76_299000"
           << "d81_098000"
           << "d86_326000"
           << "d91_937000"
           << "d97_968000"
           << "d104_455000"
           << "d111_443000"
           << "d118_979000"
           << "d127_118000"
           << "d135_920000"
           << "d145_454000"
           << "d155_798000"
           << "d167_036000"
           << "d179_268000"
           << "d192_605000"
           << "d207_171000"
           << "d223_108000"
           << "d240_576000"
           << "d259_754000"
           << "d280_845000"
           << "d304_077000"
           << "d329_706000"
           << "d358_019000"
           << "d389_336000"
           << "d424_014000"
           << "d462_451000"
           << "d505_091000"
           << "d552_426000"
           << "d605_006000"
           << "d663_443000"
           << "d728_419000"
           << "d800_699000"
           << "d839_861000";

    xyzdata xyz;
    xyz.y.push_back(8.732000);
    xyz.y.push_back(8.980000);
    xyz.y.push_back(9.498000);
    xyz.y.push_back(10.046000);
    xyz.y.push_back(10.626000);
    xyz.y.push_back(11.240000);
    xyz.y.push_back(11.890000);
    xyz.y.push_back(12.578000);
    xyz.y.push_back(13.307000);
    xyz.y.push_back(14.079000);
    xyz.y.push_back(14.896000);
    xyz.y.push_back(15.762000);
    xyz.y.push_back(16.680000);
    xyz.y.push_back(17.652000);
    xyz.y.push_back(18.682000);
    xyz.y.push_back(19.774000);
    xyz.y.push_back(20.932000);
    xyz.y.push_back(22.159000);
    xyz.y.push_back(23.461000);
    xyz.y.push_back(24.841000);
    xyz.y.push_back(26.306000);
    xyz.y.push_back(27.861000);
    xyz.y.push_back(29.512000);
    xyz.y.push_back(31.264000);
    xyz.y.push_back(33.126000);
    xyz.y.push_back(35.104000);
    xyz.y.push_back(37.207000);
    xyz.y.push_back(39.442000);
    xyz.y.push_back(41.821000);
    xyz.y.push_back(44.352000);
    xyz.y.push_back(47.047000);
    xyz.y.push_back(49.918000);
    xyz.y.push_back(52.979000);
    xyz.y.push_back(56.243000);
    xyz.y.push_back(59.726000);
    xyz.y.push_back(63.446000);
    xyz.y.push_back(67.421000);
    xyz.y.push_back(71.672000);
    xyz.y.push_back(76.299000);
    xyz.y.push_back(81.098000);
    xyz.y.push_back(86.326000);
    xyz.y.push_back(91.937000);
    xyz.y.push_back(97.968000);
    xyz.y.push_back(104.455000);
    xyz.y.push_back(111.443000);
    xyz.y.push_back(118.979000);
    xyz.y.push_back(127.118000);
    xyz.y.push_back(135.920000);
    xyz.y.push_back(145.454000);
    xyz.y.push_back(155.798000);
    xyz.y.push_back(167.036000);
    xyz.y.push_back(179.268000);
    xyz.y.push_back(192.605000);
    xyz.y.push_back(207.171000);
    xyz.y.push_back(223.108000);
    xyz.y.push_back(240.576000);
    xyz.y.push_back(259.754000);
    xyz.y.push_back(280.845000);
    xyz.y.push_back(304.077000);
    xyz.y.push_back(329.706000);
    xyz.y.push_back(358.019000);
    xyz.y.push_back(389.336000);
    xyz.y.push_back(424.014000);
    xyz.y.push_back(462.451000);
    xyz.y.push_back(505.091000);
    xyz.y.push_back(552.426000);
    xyz.y.push_back(605.006000);
    xyz.y.push_back(663.443000);
    xyz.y.push_back(728.419000);
    xyz.y.push_back(800.699000);
    xyz.y.push_back(839.861000);

    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));
    connect(&table, SIGNAL(logmessagesR(QString)), this,
            SLOT(LogMessagesR(QString)));
    std::vector<long long> time;
    QVector<std::vector<double> *> c;
    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
    table.fechvariouscolumns_timeanddoubles(tname, cnames, timei,timef, &time,&c);
    xyz.z.resize(cnames.count());
    for (int j=0; j<(int)time.size(); j++){
        xyz.x.push_back(time.at(j)/1000.);
        for (int kk = 0; kk < cnames.count(); kk++) {
            xyz.z[kk].push_back(c.at(kk)->at(j)); //factor de escala aqui.
        }
    }
    for(int i=0;i<c.count();i++){
        delete c.at(i);
    }

    QRect geo = this->ui->graphicsView->geometry();
    int xlen = geo.width();
    int ylen = geo.height();
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //  p.multiaxissep = .10;
    //  p.labelsep = 1.0;
    p.yvmin = 0.0;
    p.xvmin=0.2;
    p.Plotname = "testing proprieties";
    // xyz.p.colorscale="nocolorscale";
    xyz.p.colorscale="insidecolorscale";
    xyz.p.surfacetype="countour";
    xyz.p.ylog=true;
    xyz.p.zlog=true;
    xyz.p.zlogmin=1;
    plot.Plot_page(xlen, ylen, "memqt", "", 255, 255, 255, 1.);
    plot.SetInicialColormap(); //must be called after Plot_page that inicializes pl

    plot.SurfacePlot(timeii / 1000., timeff / 1000., &xyz, &p);

    QVector<xydata *> xy;
  xy << new xydata;

    for (long long i = timei; i < timef; i = i + 10000) {
        xy[0]->x.push_back(i / 1000.);
        xy[0]->y.push_back(5 * (i / 1000.) * (i / 1000.));
    }
    p.xvmin=0.1;
    plot.Multiaxis(timei / 1000, timef / 1000, &xy, &p);

    plot.deleteStream();


    plot.image->rgbSwapped().save("lixo100.png");
    QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
    scene.clear();
    scene.addPixmap(*renderer);
    this->ui->graphicsView->setScene(&scene);
    delete renderer;
    plot.deleteImage();

}
void MainWindow::testehisto1() {
    long long timei = 1573463400001;
    //  timef = 1573487100000;
    long long timef = 1573497100000;
    Plotting plot;
    plotproprieties p;
    connect(&plot, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));

    QString tname = "SMPS_MPSS_data";
    QStringList cnames;

    cnames.clear();
    cnames << "d8_732000"
           << "d8_980000"
           << "d9_498000"
           << "d10_046000"
           << "d10_626000"
           << "d11_240000"
           << "d11_890000"
           << "d12_578000"
           << "d13_307000"
           << "d14_079000"
           << "d14_896000"
           << "d15_762000"
           << "d16_680000"
           << "d17_652000"
           << "d18_682000"
           << "d19_774000"
           << "d20_932000"
           << "d22_159000"
           << "d23_461000"
           << "d24_841000"
           << "d26_306000"
           << "d27_861000"
           << "d29_512000"
           << "d31_264000"
           << "d33_126000"
           << "d35_104000"
           << "d37_207000"
           << "d39_442000"
           << "d41_821000"
           << "d44_352000"
           << "d47_047000"
           << "d49_918000"
           << "d52_979000"
           << "d56_243000"
           << "d59_726000"
           << "d63_446000"
           << "d67_421000"
           << "d71_672000"
           << "d76_299000"
           << "d81_098000"
           << "d86_326000"
           << "d91_937000"
           << "d97_968000"
           << "d104_455000"
           << "d111_443000"
           << "d118_979000"
           << "d127_118000"
           << "d135_920000"
           << "d145_454000"
           << "d155_798000"
           << "d167_036000"
           << "d179_268000"
           << "d192_605000"
           << "d207_171000"
           << "d223_108000"
           << "d240_576000"
           << "d259_754000"
           << "d280_845000"
           << "d304_077000"
           << "d329_706000"
           << "d358_019000"
           << "d389_336000"
           << "d424_014000"
           << "d462_451000"
           << "d505_091000"
           << "d552_426000"
           << "d605_006000"
           << "d663_443000"
           << "d728_419000"
           << "d800_699000"
           << "d839_861000";

     histodata histo;
     dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));
    connect(&table, SIGNAL(logmessagesR(QString)), this,
            SLOT(LogMessagesR(QString)));
    std::vector<long long> time;
    QVector<std::vector<double> *> c;
    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
    table.fechvariouscolumns_timeanddoubles(tname, cnames, timei,timef, &time,&c);
    for(int i=0; i<cnames.count(); i++){
        double sum=0.;
        int npoints=0;
    for (int j=0; j<(int)time.size(); j++){
        sum=sum+c.at(i)->at(j);
        npoints++;
    }
        histo.y.push_back(sum/npoints);
    }

    for(int i=0;i<c.count();i++){
        delete c.at(i);
    }
   // histo.p.period
    histo.p.htype=0;

    QString bins="8.73,,,,,,,,,,16.8,,,,,,,,,,32.2,,,,,,,,,,61.8,,,,,,,,,,119,,,,,,,,,,228,,,,,,,,,,437,,,,,,,,,,840";
    QStringList binss=bins.split(",");
    for(int i=0; i<binss.count(); i++){
    histo.x.push_back(binss.at(i));
    }
    histo.p.period="period ";
    histo.p.period.append(QString::number(timef-timei));
    histo.p.period.append(" seconds   since: ");
    histo.p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());

    QVector<histodata *> qhisto;
    qhisto <<&histo;

    QRect geo = this->ui->graphicsView->geometry();
    int xlen = geo.width();
    int ylen = geo.height();
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p.yvmin = 0.1;
    plot.Plot_page(xlen, ylen, "memqt", "", 255, 255, 255, 1.);
    plot.SetInicialColormap(); //must be called after Plot_page that inicializes pl

    plot.HistoPlot(&qhisto, &p);
    plot.deleteStream();

    QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
    scene.clear();
    scene.addPixmap(*renderer);
    this->ui->graphicsView->setScene(&scene);
    delete renderer;
    plot.deleteImage();
}

void MainWindow::LogMessages(QString mess) { ui->textBrowser->append(mess); }

void MainWindow::LogMessagesR(QString text) {
  ui->textBrowser->setTextColor("red");
  ui->textBrowser->append(text);
  ui->textBrowser->setTextColor("black");
}
void MainWindow::testehisto2() {
    long long timei = 1573463400001;
    //  timef = 1573487100000;
    long long timef = 1573497100000;
    Plotting plot;
    plotproprieties p;
    connect(&plot, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));

    QString tname = "SMPS_MPSS_data";
    QStringList cnames;

    cnames.clear();
    cnames << "d8_732000"
           << "d8_980000"
           << "d9_498000"
           << "d10_046000"
           << "d10_626000"
           << "d11_240000"
           << "d11_890000"
           << "d12_578000"
           << "d13_307000"
           << "d14_079000"
           << "d14_896000"
           << "d15_762000"
           << "d16_680000"
           << "d17_652000"
           << "d18_682000"
           << "d19_774000"
           << "d20_932000"
           << "d22_159000"
           << "d23_461000"
           << "d24_841000"
           << "d26_306000"
           << "d27_861000"
           << "d29_512000"
           << "d31_264000"
           << "d33_126000"
           << "d35_104000"
           << "d37_207000"
           << "d39_442000"
           << "d41_821000"
           << "d44_352000"
           << "d47_047000"
           << "d49_918000"
           << "d52_979000"
           << "d56_243000"
           << "d59_726000"
           << "d63_446000"
           << "d67_421000"
           << "d71_672000"
           << "d76_299000"
           << "d81_098000"
           << "d86_326000"
           << "d91_937000"
           << "d97_968000"
           << "d104_455000"
           << "d111_443000"
           << "d118_979000"
           << "d127_118000"
           << "d135_920000"
           << "d145_454000"
           << "d155_798000"
           << "d167_036000"
           << "d179_268000"
           << "d192_605000"
           << "d207_171000"
           << "d223_108000"
           << "d240_576000"
           << "d259_754000"
           << "d280_845000"
           << "d304_077000"
           << "d329_706000"
           << "d358_019000"
           << "d389_336000"
           << "d424_014000"
           << "d462_451000"
           << "d505_091000"
           << "d552_426000"
           << "d605_006000"
           << "d663_443000"
           << "d728_419000"
           << "d800_699000"
           << "d839_861000";

    histodata histo,histo1;
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, SIGNAL(logmessages(QString)), this,
            SLOT(LogMessages(QString)));
    connect(&table, SIGNAL(logmessagesR(QString)), this,
            SLOT(LogMessagesR(QString)));
    std::vector<long long> time;
    QVector<std::vector<double> *> c;
    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
    table.fechvariouscolumns_timeanddoubles(tname, cnames, timei,timef, &time,&c);
    for(int i=0; i<10; i++){
       // for(int i=0; i<cnames.count(); i++){
        double sum=0.;
        int npoints=0;
        for (int j=0; j<(int)time.size(); j++){
            sum=sum+c.at(i)->at(j);
            npoints++;
        }
        histo.y.push_back(sum/npoints);
    }
    for(int i=10; i<20; i++){
        // for(int i=0; i<cnames.count(); i++){
        double sum=0.;
        int npoints=0;
        for (int j=0; j<(int)time.size(); j++){
            sum=sum+c.at(i)->at(j);
            npoints++;
        }
        histo1.y.push_back(sum/npoints);
    }


    for(int i=0;i<c.count();i++){
        delete c.at(i);
    }
    // histo.p.period
    histo.p.htype=2; //0 histo consecutivos 1 e 2 histos sobrepostos 2 com linhas.

    histo.x.push_back("h0");
    histo.x.push_back("h1");
    histo.x.push_back("h2");
    histo.x.push_back("h3");
    histo.x.push_back("h4");
    histo.x.push_back("h5");
    histo.x.push_back("h6");
    histo.x.push_back("h7");
    histo.x.push_back("h8");
    histo.x.push_back("h9");
    histo1.x.push_back("V0");
    histo1.x.push_back("V1");
    histo1.x.push_back("V2");
    histo1.x.push_back("V3");
    histo1.x.push_back("V4");
    histo1.x.push_back("V5");
    histo1.x.push_back("V6");
    histo1.x.push_back("V7");
    histo1.x.push_back("V8");
    histo1.x.push_back("V9");
    histo1.p.histocolor="OOOOFF";

    histo.p.period="period ";
    histo.p.period.append(QString::number(timef-timei));
    histo.p.period.append(" seconds   since: ");
    histo.p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());

    QVector<histodata *> qhisto;
    qhisto <<&histo;
    qhisto <<&histo1;

    QRect geo = this->ui->graphicsView->geometry();
    int xlen = geo.width();
    int ylen = geo.height();
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p.yvmin = 0.1;
    plot.Plot_page(xlen, ylen, "memqt", "", 255, 255, 255, 1.);
    plot.SetInicialColormap(); //must be called after Plot_page that inicializes pl

    plot.HistoPlot(&qhisto, &p);
    plot.deleteStream();

    QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
    scene.clear();
    scene.addPixmap(*renderer);
    this->ui->graphicsView->setScene(&scene);
    delete renderer;
    plot.deleteImage();
}
