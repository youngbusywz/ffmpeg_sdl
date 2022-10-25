#include "Widget.h"
#include "ui_Widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_spVideoDecode = std::make_shared<VideoDecode>();

    m_spVideoDecode->Init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QString playUrl = ui->lineEdit->text();
    m_spVideoDecode->OpenInput(playUrl.toLocal8Bit().data());
    m_spVideoDecode->Play((void*)(ui->frame->winId()));
}
