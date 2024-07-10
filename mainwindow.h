#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>
#include <QDebug>
#include <QString>
#include <QTimer>

QT_BEGIN_NAMESPACE


using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void onT2();
private slots:
    void onT1();
private:
    Ui::MainWindow *ui;
    int InitMav();
    static void CallBack_RC_Channels(const mavlink_message_t msg_raw);
    static void CallBack_Battery(Telemetry::Battery btry);

    QTimer *tt1;
    QTimer *tt2;

};
#endif // MAINWINDOW_H
