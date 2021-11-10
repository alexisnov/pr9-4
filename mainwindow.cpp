#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    com = new Communicator();
    //Связь сигналов и слотов
    connect(com,&Communicator::resp_countries,this,&MainWindow::resp_countries);
    connect(com,&Communicator::resp_summary,this,&MainWindow::resp_summary);
    connect(com,&Communicator::resp_country_info,this,&MainWindow::resp_country);
    connect(com,&Communicator::resp_city_info,this,&MainWindow::resp_city_info);
    //Инициализация таймера
    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(t_tick()));
    QChart *chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->plotLayout->addWidget(chartView);
    //Инициализация вектора
    city_case = new QVector<int>;
    country_case = new QVector<int>;
    cityName = "Moscow";
    countryName = "Russia";
    loadCountries();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_request_clicked()
{
    //com->getCountries();
    com->getSummary();
    com->getCountryInfo(countryName,ui->dateEdit->text());
    com->getCityInfo(cityName,ui->dateEdit->text());
}


//Слот получения списка стран
void MainWindow::resp_countries(QByteArray resp)
{
    displayCountries(resp);
    //Запись в файл
    if(!countriesFile->open(QIODevice::WriteOnly))
    {
        qDebug() << "Ошибка открытия для чтения";
    }
     countriesFile->write(resp);
    countriesFile->close();
}

//Слот получения сводки
void MainWindow::resp_summary(QByteArray resp)
{
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(resp);
      // Забираем из документа корневой объект
      QJsonObject root = document.object();
      ui->textEdit->clear();
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
       //Проверка входных данных
      if(root.keys().size()>0 && root["data"]!=NULL){
          //Сводка по миру
          QJsonObject global = root["data"].toObject();
          ui->textEdit->setText(global.value("confirmed").toString());
          //Данные о заболеваемости по миру
          int world_issues = global.value("confirmed").toInt();
          ui->lcdNumber_world->display(world_issues);
      }else{
          ui->textEdit->setText("Ошибка получения данных");
      }
}

//Слот получения данных по стране
void MainWindow::resp_country(QByteArray resp)
{
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(resp);
      // Забираем из документа корневой объект
      QJsonObject root_country = document.object();
      ui->textEdit->clear();
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
      //Проверка входных данных
     if(root_country.keys().size()>0 && root_country["data"]!=NULL){
         //Сводка по городу
         QJsonArray global = root_country["data"].toArray();
         //Данные о заболеваемости по городу
         int country_issues = 0;
         //Сумма по всем регионам страны
         for (int i=0;i<global.size();i++) {
             QJsonObject country = global[i].toObject();
             ui->textEdit->setText(country.value("confirmed").toString());
             country_issues+=country.value("confirmed").toInt();
         }
         if(plot){//Добавление данных для графика
                //country_case->append(country_issues);
                //request = false;
                QPoint point = QPoint(dayID,country_issues);
                seriesList[reqID]->append(point);
         }
         ui->lcdNumber_rus->display(country_issues);

     }else{
         ui->textEdit->setText("Ошибка получения данных");
     }

}

//Слот получения данных по городу
void MainWindow::resp_city_info(QByteArray resp)
{
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(resp);
      // Забираем из документа корневой объект
      QJsonObject root_city = document.object();
      ui->textEdit->clear();
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
      //Проверка входных данных
     if(root_city.keys().size()>0 && root_city["data"]!=NULL){
         //Сводка по городу
         QJsonArray global = root_city["data"].toArray();
         QJsonObject city = global[0].toObject();
         ui->textEdit->setText(city.value("confirmed").toString());
         //Данные о заболеваемости по городу
         int city_issues = city.value("confirmed_diff").toInt();
         ui->lcdNumber_mos->display(city_issues);
         if(plot){//Добавление данных для графика
                city_case->append(city_issues);
                request = false;

         }
     }else{
         ui->textEdit->setText("Ошибка получения данных");
     }
}

void MainWindow::t_tick(){
    if(reqID<reqList.size()-1){//Проверка граничных условий
        if(dayID<daysN){
            if(!request){
                QDate date = date_start.addDays(dayID);
                com->getCityInfo(cityName,date.toString("yyyy-MM-dd"));
                com->getCountryInfo(reqList[reqID],date.toString("yyyy-MM-dd"));
                request = true;
                dayID++;
                int p = (int) dayID*100.0/daysN;
                //Обновить значение строки прогресса
                ui->progressBar->setValue(p);
            }
        }else{
            dayID=0;
            reqID++;
        }
    }else{
        //Построение графика
        chart = new QChart();
        chart->setTitle("Динамика заболеваемости");
        foreach(QLineSeries *series,seriesList){
            chart->addSeries(series);
        }
        //chart->legend()->hide();
        chart->createDefaultAxes();
        chartView->setChart(chart);
        //Завершаем запросы
        request=0;
        plot=0;
        ui->progressBar->setEnabled(false);
        timer->stop();
    }
}

void MainWindow::on_pushButton_plot_clicked()
{
    date_start = ui->dateEdit_start->date();
    date_end = ui->dateEdit_end->date();
    daysN = date_start.daysTo(date_end)+1;
    //Поднять флаг "plot"
    plot = true;
    request = false;//флаг "request" - сброс
    dayID = 0;//сброс ID дня
    city_case->clear();
    country_case->clear();
    timer->start(100);
    //Сброс строки прогресса
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(true);
    reqID=0;
    //Инициализация списка рядов данных
    seriesList.clear();
    for(int i=0;i<reqList.size();i++){
        QLineSeries *series = new QLineSeries(chart);
        series->setName(reqList[i]);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    //Сохранение идентификатора страны
    countryName = arg1;
}

void MainWindow::loadCountries(){
    //Проверка на существование файла
    countriesFile = new QFile("regions.json");
    if(countriesFile->exists()){//Чтение информации из файла
        if(!countriesFile->open(QIODevice::ReadOnly))
        {
            qDebug() << "Ошибка открытия для чтения";
        }
        displayCountries(countriesFile->readAll());
    }else{//Запрос от сервера
        com->getCountries();
    }
}

void MainWindow::displayCountries(QByteArray text){
    //Очистка списка
    ui->comboBox->clear();
    ui->treeWidget->clear();
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(text);

      // Забираем из документа корневой объект
      QJsonObject root = document.object();
      if(root.keys().size()>0 && root["data"]!=NULL){
        QJsonArray countries = root["data"].toArray();
        foreach(QJsonValue country, countries){
            QJsonObject c = country.toObject();
            //Наполнение списка на вкладке 1
            ui->comboBox->addItem(c["name"].toString());
            //Наполенение дерева регионов на вкладке 2
            QStringList strList = QStringList();
            strList.append(c["name"].toString());
            QTreeWidgetItem *item = new QTreeWidgetItem(strList);
            ui->treeWidget->addTopLevelItem(item);
            ui->treeWidget->sortItems(0,Qt::SortOrder::AscendingOrder);
        }

      }
      ui->textEdit->clear();
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

}

void MainWindow::on_treeWidget_itemSelectionChanged()
{
    //Формирование очереди запросов
    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    qDebug() << "-----";
    reqList.clear();
    foreach(QTreeWidgetItem *item,items){
        qDebug() << item->text(0);
        reqList.append(item->text(0));
    }
}
