#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <communicator.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Communicator* com;//Объект соединения с сервером
    //Хранение запросов по городу
    QVector<int> *city_case;
    //Хранение запросов по стране
    QVector<int> *country_case;
    bool plot;//Флаг построения
    bool request;//Флаг на текущий запрос
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_request_clicked();
    //Список стран
    void resp_countries(QByteArray resp);
    //Получение сводки
    void resp_summary(QByteArray resp);
    //Получение данных по стране
    void resp_country(QByteArray resp);
    //Получение даных по городу
    void resp_city_info(QByteArray resp);
    //Тик таймера
    void t_tick();
    void on_pushButton_plot_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QDate date_start;//Дата начала построений
    QDate date_end;//Дата конца построений
    int daysN;//Кол-во дней для построений
    int dayID;//ID дня
    QString countryName;//Идентификатор страны
    QString cityName;//Идентификатор города
    //График
    QChart *chart;
    QLineSeries *series;//Ряд данных
    QChartView *chartView;//Виджет
};
#endif // MAINWINDOW_H
