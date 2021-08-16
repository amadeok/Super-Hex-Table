#include "settings_window.h"
#include "ui_settings_window.h"

settings_window::settings_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings_window)
{
    ui->setupUi(this);
    read_conf_file();
    ui->input_separator_text->setText(input_separator);
    ui->output_separator_text->setText(output_separator);
    if (enable_dark_theme)
        ui->dark_theme_button->setChecked(true);
    else  ui->dark_theme_button->setChecked(false);


}

settings_window::~settings_window()
{
    delete ui;
}

void settings_window::read_conf_file()
{
    qDebug() << "read_conf_file";

    bool ok;

    QFile file( file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {   qDebug() << "unable to open configuration file";
        write_conf_file(true);
        return;
    }
    else qDebug() << "configuration file opened " << file_path;

    QTextStream in(&file);
    QString input_separator_ = in.readLine();
    QString output_separator_ = in.readLine();
    enable_dark_theme = in.readLine().toInt(&ok, 10);
    if (!input_separator_.size() || !output_separator_.size())
        write_conf_file(true);
    else {
        input_separator = input_separator_;
        output_separator = output_separator_;
    }
    file.close();

}

int settings_window::write_conf_file(bool write_defaults)
{
    qDebug() << "write_conf_file";

    QFile file( file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "unable to open file for writing";
        return 1;
    }

    QTextStream out(&file);
    QByteArray arr;
    //arr.append(input_separator.data());
    if (!write_defaults){
        QString text = ui->input_separator_text->text();
        if (text.size() == 1)
            input_separator = ui->input_separator_text->text();
        else  { QToolTip::showText(ui->input_separator_text->mapToGlobal(QPoint()), "Separator must be exactly 1 character long");return 0;}
        text = ui->output_separator_text->text();
        if (text.size() == 1)
            output_separator = ui->output_separator_text->text();
        else{   QToolTip::showText(ui->output_separator_text->mapToGlobal(QPoint()), "Separator must be exactly 1 character long"); return 0;}

        if (ui->dark_theme_button->isChecked())
            enable_dark_theme = 1;
        else enable_dark_theme = 0;
    }


    QString combined = input_separator + "\n" + output_separator + "\n" + QString::number(enable_dark_theme);

    out << combined;

    file.close();
    return 1;
}




void settings_window::on_accept_button_clicked()
{
  if (write_conf_file(false))
      hide();
}


void settings_window::on_cancel_button_clicked()
{
    hide();
}

