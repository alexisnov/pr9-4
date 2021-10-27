/**
  @file
  @brief файл класса Communicator
  @page Communicator -
  */
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H
#include <qstring.h>
#include <QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

/**
 * @brief Класс для доступа к внешнему API
 * @details Данный класс реализует доступ к веб-сервису получния данных о заболеваемости COVID.
 * @authors МИРЭА
 * @version 0.1
 * @date 31.10.2020
 * @copyright GPL 3.0
 * @todo Релизовать запросы на все типы данных (запрос регионов стран, запрос заболеваемости по датам и региону)
 * @warning Данный класс создан в учебных целях
 */

class Communicator : public QObject
{
    Q_OBJECT
    QString base_url;
    int reqID = 0; //ID запроса
    QVector<int> reqIDs;
public:
    /**
      * @brief Конструктор класса без параметров
      */
     Communicator();
     /**
       * @brief Конструктор класса с параметром URL
       * @param[in] url Адрес для API веб-сервиса
       */
     Communicator(QString url);

public slots:
    /**
     * @brief Получение списка стран
     */
    /** Пример использования
     * @code
     * com->getCountries();
     * @endcode
     */
    void getCountries();
    /**
     * @brief Получение сводной информации по миру
     */
    void getSummary();
    /**
     * @brief Получение сводной информации по стране
     */
    void getCountryInfo(QString c_code, QString date);
    /**
     * @brief Получение сводной информации по городу
     */
    void getCityInfo(QString c_code, QString date);
    /**
     * @brief Формирование запросa
     * @param[in] reply Ответ от веб-сервиса
     *
     */
    void getResponse(QNetworkReply *reply);
    /**
      * @brief Перенаправление сигналов
      */
    void summary_relay(QNetworkReply *reply);
    void country_relay(QNetworkReply *reply);
    void city_relay(QNetworkReply *reply);

signals:
    /**
     * @brief Сигнал передачи результатов запроса
     * @param[in] str Ответ от веб-сервиса по странам
     */
    void resp_countries(QByteArray str);
    /**
     * @brief Сигнал передачи результатов запроса
     * @param[in] str Ответ от веб-сервиса по сводке
     */
    void resp_summary(QByteArray str);
    /**
     * @brief Результат запроса на данные по стране
     */
    void resp_country_info(QByteArray str);
    /**
     * @brief Результат запроса на данные по городу
     */
    void resp_city_info(QByteArray str);

};

#endif // COMMUNICATOR_H
