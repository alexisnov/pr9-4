#include "communicator.h"

Communicator::Communicator()
{
    base_url = "https://covid-api.com/api/";
}

Communicator::Communicator(QString url)
{
    base_url = url;
}

void Communicator::getCountries()
{
    //Базовый адрес для запроса
    QString url = base_url+"regions";
    reqID =1;
    //Добавление запроса в очередь
    reqIDs.push_back(reqID);
    //Запрос
    QNetworkRequest request =  QNetworkRequest(QUrl(url));
    //Объект доступа - содержит общую конфигурацию и настройки для посылаемых запросов
    QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
    //Связь слотов и сигналов
    connect(mngr, &QNetworkAccessManager::finished,this,&Communicator::getResponse);
    mngr->get(request);
}

void Communicator::getSummary()
{
    reqID=2;
    //Добавление запроса в очередь
    reqIDs.push_back(reqID);
    //Базовый адрес для запроса
    QString url = base_url+"reports/total";
    //Запрос
    QNetworkRequest request =  QNetworkRequest(QUrl(url));
    //Объект доступа - содержит общую конфигурацию и настройки для посылаемых запросов
    QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
    //Связь слотов и сигналов
    connect(mngr, &QNetworkAccessManager::finished,this,&Communicator::summary_relay);
    mngr->get(request);
}

void Communicator::getCountryInfo(QString c_code, QString date)
{
    reqID=3;
    //Добавление запроса в очередь
    reqIDs.push_back(reqID);
    //Базовый адрес для запроса
    QString url = base_url+"reports?date="+date+"&q="+c_code;
    //Запрос
    QNetworkRequest request =  QNetworkRequest(QUrl(url));
    //Объект доступа - содержит общую конфигурацию и настройки для посылаемых запросов
    QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
    //Связь слотов и сигналов
    connect(mngr, &QNetworkAccessManager::finished,this,&Communicator::country_relay);
    mngr->get(request);
}

void Communicator::getCityInfo(QString c_code, QString date)
{
    reqID=4;
    //Добавление запроса в очередь
    reqIDs.push_back(reqID);
    //Базовый адрес для запроса
    QString url = base_url+"reports?date="+date+"&q="+c_code;
    //Запрос
    QNetworkRequest request =  QNetworkRequest(QUrl(url));
    //Объект доступа - содержит общую конфигурацию и настройки для посылаемых запросов
    QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
    //Связь слотов и сигналов
    connect(mngr, &QNetworkAccessManager::finished,this,&Communicator::city_relay);
    mngr->get(request);
}

void Communicator::getResponse(QNetworkReply *reply)
{
    QByteArray str = reply->readAll();
    qDebug() << str;
    emit resp_countries(str);

}

void Communicator::summary_relay(QNetworkReply *reply){
    QByteArray str = reply->readAll();
    qDebug() << str;
    emit resp_summary(str);
}


void Communicator::country_relay(QNetworkReply *reply){
    QByteArray str = reply->readAll();
    qDebug() << str;
    emit resp_country_info(str);
}

void Communicator::city_relay(QNetworkReply *reply){
    QByteArray str = reply->readAll();
    qDebug() << str;
    emit resp_city_info(str);
}
