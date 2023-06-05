#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Машины
    QHBoxLayout *carLayout = new QHBoxLayout;
    carBrandLineEdit = new QLineEdit;
    carCapacityLineEdit = new QLineEdit;
    QPushButton *addCarButton = new QPushButton("Добавить машину");
    connect(addCarButton, &QPushButton::clicked, this, &MainWindow::addCar);

    carLayout->addWidget(new QLabel("Марка машины:"));
    carLayout->addWidget(carBrandLineEdit);
    carLayout->addWidget(new QLabel("Грузоподъемность (кг):"));
    carLayout->addWidget(carCapacityLineEdit);
    carLayout->addWidget(addCarButton);

    mainLayout->addLayout(carLayout);

    // Грузы
    QHBoxLayout *cargoLayout = new QHBoxLayout;
    cargoSourceLineEdit = new QLineEdit;
    cargoDestinationLineEdit = new QLineEdit;
    cargoWeightLineEdit = new QLineEdit;
    QPushButton *addCargoButton = new QPushButton("Добавить груз");
    connect(addCargoButton, &QPushButton::clicked, this, &MainWindow::addCargo);

    cargoLayout->addWidget(new QLabel("Город отправления:"));
    cargoLayout->addWidget(cargoSourceLineEdit);
    cargoLayout->addWidget(new QLabel("Город назначения:"));
    cargoLayout->addWidget(cargoDestinationLineEdit);
    cargoLayout->addWidget(new QLabel("Вес груза (кг):"));
    cargoLayout->addWidget(cargoWeightLineEdit);
    cargoLayout->addWidget(addCargoButton);

    mainLayout->addLayout(cargoLayout);

    // Таблицы
    QHBoxLayout *tablesLayout = new QHBoxLayout;
    carTableWidget = new QTableWidget;
    carTableWidget->setColumnCount(2);
    carTableWidget->setHorizontalHeaderLabels(QStringList() << "Марка машины"
                                                  << "Грузоподъемность");
    tablesLayout->addWidget(carTableWidget);

    cargoTableWidget = new QTableWidget;
    cargoTableWidget->setColumnCount(3);
    cargoTableWidget->setHorizontalHeaderLabels(QStringList() << "Город отправления"
                                                             << "Город назначения"
                                                             << "Вес груза");
    tablesLayout->addWidget(cargoTableWidget);

    mainLayout->addLayout(tablesLayout);

    // Результат
    resultLabel = new QLabel;
    mainLayout->addWidget(resultLabel);

    mainLayout->addStretch();

    distributeButton = new QPushButton("Распределить грузы");
    connect(distributeButton, &QPushButton::clicked, this, &MainWindow::distributeCargo);

    mainLayout->addWidget(distributeButton);

    resultTableWidget = new QTableWidget;
    resultTableWidget->setColumnCount(2);
    resultTableWidget->setHorizontalHeaderLabels(QStringList() << "Машина" << "Груз");
    mainLayout->addWidget(resultTableWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::addCar()
{
    QString carBrand = carBrandLineEdit->text();
    QString carCapacity = carCapacityLineEdit->text();

    int row = carTableWidget->rowCount();
    carTableWidget->insertRow(row);
    carTableWidget->setItem(row, 0, new QTableWidgetItem(carBrand));
    carTableWidget->setItem(row, 1, new QTableWidgetItem(carCapacity));

    carBrandLineEdit->clear();
    carCapacityLineEdit->clear();
}

void MainWindow::addCargo()
{
    QString cargoSource = cargoSourceLineEdit->text();
    QString cargoDestination = cargoDestinationLineEdit->text();
    QString cargoWeight = cargoWeightLineEdit->text();

    int row = cargoTableWidget->rowCount();
    cargoTableWidget->insertRow(row);
    cargoTableWidget->setItem(row, 0, new QTableWidgetItem(cargoSource));
    cargoTableWidget->setItem(row, 1, new QTableWidgetItem(cargoDestination));
    cargoTableWidget->setItem(row, 2, new QTableWidgetItem(cargoWeight));

    cargoSourceLineEdit->clear();
    cargoDestinationLineEdit->clear();
    cargoWeightLineEdit->clear();
}

void MainWindow::distributeCargo()
{
    int carCount = carTableWidget->rowCount();
    int cargoCount = cargoTableWidget->rowCount();

    QVector<int> carCapacities;
    QVector<int> cargoWeights;
    QVector<int> assignedCars(cargoCount, -1); // Индексы машин, к которым назначены грузы
    QVector<bool> usedCars(carCount, false); // Флаги использования машин

    // Получение грузоподъемностей машин
    for (int i = 0; i < carCount; ++i) {
        QTableWidgetItem *capacityItem = carTableWidget->item(i, 1);
        int capacity = capacityItem->text().toInt();
        carCapacities.append(capacity);
    }

    // Получение весов грузов
    for (int i = 0; i < cargoCount; ++i) {
        QTableWidgetItem *weightItem = cargoTableWidget->item(i, 2);
        int weight = weightItem->text().toInt();
        cargoWeights.append(weight);
    }

    // Распределение грузов на машины
    QVector<int> unassignedCargo; // Вектор для хранения индексов грузов без машин
    for (int i = 0; i < cargoCount; ++i) {
        bool assigned = false; // Флаг для отслеживания, был ли груз назначен машине

        for (int j = 0; j < carCount; ++j) {
            if (!usedCars[j] && assignedCars[i] == -1 && cargoWeights[i] <= carCapacities[j]) {
                assignedCars[i] = j;
                carCapacities[j] -= cargoWeights[i];
                usedCars[j] = true; // Отметить машину как использованную
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            unassignedCargo.append(i); // Добавить индекс груза без машины
        }
    }

    // Очистка таблицы результатов
    resultTableWidget->clearContents();
    resultTableWidget->setRowCount(0);

    // Вывод результата
    for (int i = 0; i < cargoCount; ++i) {
        if (assignedCars[i] != -1) {
            QTableWidgetItem *brandItem = carTableWidget->item(assignedCars[i], 0);
            QTableWidgetItem *sourceItem = cargoTableWidget->item(i, 0);
            QTableWidgetItem *destinationItem = cargoTableWidget->item(i, 1);
            QTableWidgetItem *weightItem = cargoTableWidget->item(i, 2);

            QString carBrand = brandItem->text();
            QString cargoSource = sourceItem->text();
            QString cargoDestination = destinationItem->text();
            QString cargoWeight = weightItem->text();

            int row = resultTableWidget->rowCount();
            resultTableWidget->insertRow(row);
            resultTableWidget->setItem(row, 0, new QTableWidgetItem(carBrand));
            resultTableWidget->setItem(row, 1, new QTableWidgetItem(cargoSource + " - " + cargoDestination + " (Вес: " + cargoWeight + " кг)"));
        }
    }

    // Вывод неназначенных грузов
    QString unassignedCargoText;
    if (!unassignedCargo.isEmpty()) {
        unassignedCargoText = "Неназначенные грузы:\n";
        for (int i = 0; i < unassignedCargo.size(); ++i) {
            QTableWidgetItem *sourceItem = cargoTableWidget->item(unassignedCargo[i], 0);
            QTableWidgetItem *destinationItem = cargoTableWidget->item(unassignedCargo[i], 1);
            QTableWidgetItem *weightItem = cargoTableWidget->item(unassignedCargo[i], 2);

            QString cargoSource = sourceItem->text();
            QString cargoDestination = destinationItem->text();
            QString cargoWeight = weightItem->text();

            unassignedCargoText += "- " + cargoSource + " - " + cargoDestination + " (Вес: " + cargoWeight + " кг)\n";
        }

        resultLabel->setText(unassignedCargoText);
    } else {
        resultLabel->clear();
    }
}
