#ifndef MAINWINDOW
#define MAINWINDOW

#include <QWidget>
#include <QTcpServer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
  void onBtnClicked();
  void onNewConnection();

private:
  Ui::MainWindow *ui_;
  QTcpServer *tcp_server;

};

#endif // MAINWINDOW
