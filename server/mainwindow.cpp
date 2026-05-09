#include "mainwindow.h"
#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *central = new QWidget(this);
    layout = new QVBoxLayout(central);
    central->setFixedSize(640, 480);

    QPushButton *button = new QPushButton("Start");
    button->setFixedSize(100, 100);
    layout->addWidget(button);

    setCentralWidget(central);

    connect(&server, &ServerHost::clientListChanged, this, &MainWindow::LogConnectionBlock);

    connect(button, &QPushButton::clicked, [this, button]() {
        if (button->text() == "Start") {
            server.start();
            button->setText("Stop");
        } else {
            server.stop();
            button->setText("Start");
            LogConnectionBlock();
        }
    });
}

void MainWindow::LogConnectionBlock() {
    for (QLabel* label : iplabels) {
        layout->removeWidget(label);
        delete label;
    }
    iplabels.clear();

    char** ips = server.getIpAddressList();
    for (int i = 0; ips[i] != nullptr; i++) {
        QLabel* label = new QLabel(ips[i]);
        iplabels.append(label);
        layout->addWidget(label);
        delete[] ips[i];
    }
    delete[] ips;
}

MainWindow::~MainWindow()
{
    delete ui;
}
