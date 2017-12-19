#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //instância da QSerialPort
    this->serialPort = new QSerialPort;

    //Montagem da lista de portas e alimentação do combobox com a lista de portas disponíveis
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    int i = 0;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos){
        ports.insert(i,serialPortInfo.portName());
        i++;
    }
    ui->comboBox_port->insertItems(0,ports);

    //Criação da lista de velocidades da comunicação serial e alimentação do combobox baudrate
    QStringList bauds;
    bauds << "9600" << "19200" << "38400" << "57600" << "115200";
    ui->comboBox_baud->insertItems(0,bauds);

    /*Conexão do clique do botão com o slot que inicia a conexão, visto no tutorial 01 de Qt*/
    connect(ui->pushButton_connect,SIGNAL(clicked(bool)),this,SLOT(connectToSerial()));

    /*Conexão do sinal "pronto para leitura" com o slot que faz a leitura. Desse modo, quando
    é feita uma leitura, o método sai ao terminá-la. Adicionalmente estamos escrevendo para a
    porta serial, enviando um Byte para acender ou apagar o LED no Arduino.
    Em alguns casos pode ser necessário utilizar threads, depende de  diversos  fatores,  mas
    normalmente dá pra resolver muita coisa só utilizando SIGNAL & SLOT.*/
    connect(this->serialPort,SIGNAL(readyRead()),this,SLOT(startReading()));

}

void MainWindow::startReading()
{
    //Faz uma leitura. Tenha em mente que esse código é um exemplo e não temos aqui os
    //devidos tratamentos de excessões.
    QByteArray readData = this->serialPort->readAll();
    //Apenas exibindo a leitura em stdout.
    qDebug() << readData;
    //A cada chamada enviamos um estado diferente para o LED, para fazê-lo piscar.
    if (this->ON){
        this->ON = false;
        this->serialPort->write("1\n");
    }
    else{
        this->ON = true;
        this->serialPort->write("0\n");
    }
}

void MainWindow::connectToSerial()
{
    /*Na conexão fazemos a abertura ou fechamento da porta, conforme descrito no video 03
     * dos tutoriais relacionados a esse exemplo.*/
    if (this->serialPort->isOpen()){
        this->serialPort->close();
        ui->label_status->setText("Desconectado");
        ui->pushButton_connect->setText("Conectar");
        return;
    }
    //se for para abrir, pegamos os parâmetros de velocidade e porta a abrir.
    this->serialPort->setPortName(ui->comboBox_port->currentText());
    this->serialPort->setBaudRate(ui->comboBox_baud->currentText().toUInt());

    /*Dentro desse método definimos o modo de abertura. Até o tutorial 03 estavamos
     * usando o modo somente-leitura. Agora precisamos abrir em modo de leitura-e-escrita,
     * porque estamos enviando o comando para acender o LED no Arduino.
     * O exemplo de escrita dentro do mesmo método que lê é apenas para simplificar, mas
     * podemos fazer a escrita de forma assíncrona, de modo que poderiamos criar botões para
     * acender LED, acionar relés, pegar status de sensores etc. O Arduino também poderia
     * responder apenas quando solicitado. Veremos essas variáveis em outro tutorial.
    */
    if (!this->serialPort->open(QIODevice::ReadWrite)){
        ui->label_status->setText("Falha ao tentar conectar");
        return;
    }
    ui->label_status->setText("Conectado");
    ui->pushButton_connect->setText("Desconectar");

    emit startReading(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
