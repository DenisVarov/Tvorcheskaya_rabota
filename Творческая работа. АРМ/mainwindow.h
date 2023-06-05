#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addCar();
    void addCargo();
    void distributeCargo();

private:
    QPushButton *distributeButton;
    QTableWidget *tableWidget;
    QLineEdit *carBrandLineEdit;
    QLineEdit *carCapacityLineEdit;
    QLineEdit *cargoSourceLineEdit;
    QLineEdit *cargoDestinationLineEdit;
    QLineEdit *cargoWeightLineEdit;
    QLabel *resultLabel;
    QTableWidget *resultTableWidget;
    QTableWidget *carTableWidget;
    QTableWidget *cargoTableWidget;

};

#endif // MAINWINDOW_H
