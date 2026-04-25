#include "mainwindow.h"
#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    central->setFixedSize(640, 480);

    QPushButton *button = new QPushButton("Start");
    button->setFixedSize(100, 100);
    layout->addWidget(button);

    setCentralWidget(central);

    char** ips = server.getIpAddressList();
    for (int i = 0; i < 10; i++) {
        if (ips[i] == nullptr) break;
        layout->addWidget(new QLabel(ips[i]));
    }

    connect(button, &QPushButton::clicked, [this, button]() {
        if (button->text() == "Start") {
            server.start();
            button->setText("Stop");
        } else {
            server.stop();
            button->setText("Start");
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
