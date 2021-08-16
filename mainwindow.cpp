#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings_window.h"

#include <QButtonGroup>
#include <QToolTip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->hex_button->setChecked(true);
    ui->BE_button->setChecked(true);
    settings_win = new settings_window;
    update_box_bytes();
}

MainWindow::~MainWindow()
{
    delete ui;
}


int MainWindow::get_input_format(){
    int fmt = 0;
    QString depth;
    int ret, index;
    bool ok;

    if (ui->text_button->isChecked())
    {      fmt = 3;
        this->t.input_encoding = ui->input_option_menu->itemText(ui->input_option_menu->currentIndex());
        this->t.dec_bit_depth = 8;
        qDebug() << "update_table Current input_encoding " << t.input_encoding;
    }
    else{
        depth = ui->input_option_menu->itemText(ui->input_option_menu->currentIndex());
        ret = ui->input_option_menu->count();
        index = ui->input_option_menu->currentIndex();
        if (index == -1)
            ui->input_option_menu->setCurrentIndex(0);
        this->t.dec_bit_depth = ui->input_option_menu->itemText(ui->input_option_menu->currentIndex()).split(" ")[0].toInt(&ok, 10);
        qDebug() << "Current bit depth " << t.dec_bit_depth;

        if (ui->hex_button->isChecked())
            fmt = 0;
        else if (ui->dec_button->isChecked())
            fmt = 1;
        else if (ui->binary_button->isChecked())
            fmt = 2;
    }
    if( t.dec_bit_depth == 0)
        qDebug() << "error bit depth";
    t.current_mode = t.input_format_;

    return fmt;
}

void MainWindow::clear_input()
{
    t.string_input = "";
    t.previous_string_input = "";

    ui->input_box->blockSignals(true);
    ui->input_box->setText("");
    ui->input_box->blockSignals(false);
    // qDebug() << "clear_input prev" << t.previous_string_input << " curr " << t.string_input;


}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->input_box && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        qDebug() << "pressed"<< key->key();
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::display_error(QString error_msg)
{
    QToolTip::showText(ui->input_box->mapToGlobal(QPoint()), error_msg);
    t.string_input = t.previous_string_input;
    ui->input_box->setText(t.previous_string_input);
    //  qDebug() << "prev " << t.previous_string_input << " curr " << t.string_input;
}

QString MainWindow::group_digits(QString input, int divider)
{
    int block_size =(8 / divider);
    //int block_size =(t.dec_bit_depth / divider);

    while (input.size() % block_size != 0)
        input = "0" + input;
    int pos = input.size();
    int nb_times = input.size() / block_size -1;
    for (int h = 0 ; h < nb_times; h++)
    {input.insert(pos-=block_size, settings_win->output_separator);}
    return input;
}

void MainWindow::update_table(QString from){
    qDebug() << "update_table from " + from ;

    t.previous_string_input = t.string_input;
    t.string_input =  ui->input_box->text();
    t.separator = settings_win->input_separator;

    int cursor_pos = ui->input_box->cursorPosition();

    if (ui->BE_button->isChecked())
        t.endiannes = big_;
    else t.endiannes = little_;

    t.input_format_ = get_input_format();

    t.buffer_preview_mode = ui->buffer_preview_menu->currentIndex();

    check_string();

    ui->input_box->setCursorPosition(cursor_pos);

    QString ret = t.get_variants();
    if( ret.size())
    {
        qDebug() << "Warning error ";
        display_error(ret);
        return;
    }
    else QToolTip::hideText();

    ui->uint8->setText(QString::number(t.uint8));
    ui->uint16->setText(QString::number(t.uint16));
    ui->uint32->setText(QString::number(t.uint32));
    ui->uint64->setText(QString::number(t.uint64));

    ui->int8->setText(QString::number(t.int8));
    ui->int16->setText(QString::number(t.int16));
    ui->int32->setText(QString::number(t.int32));
    ui->int64->setText(QString::number(t.int64));

    ui->float_2->setText(QString::number(t.float_));
    ui->double_2->setText(QString::number(t.double_));

    ui->octal->setText(t.octal);

    if (ui->group_digits->isChecked())
    {
        t.hex = group_digits(t.hex, 4);
        t.binary = group_digits(t.binary, 1);
    }
    ui->hex->setText(t.hex);

    ui->binary->setText(t.binary);

    ui->buffer_preview->setText(t.buffer_preview);

    int index = ui->encoding_menu->currentIndex();

    if (index == 0) //utf8
        ui->Text->setText(t.utf8);
    else if (index == 1) //utf16
        ui->Text->setText(t.utf16);
}

void MainWindow::check_string(){
    QString reformatted = "";
    int base = 2;

    if (ui->text_button->isChecked()){
        bool exceeded  = false;
        if (t.input_encoding == "UTF-8")
        {
            t.utf8_buffer.clear();
            t.utf8_buffer = t.string_input.toUtf8();
            if (t.utf8_buffer.size()*sizeof(char)> 8)
            {t.string_input = t.previous_string_input; exceeded = true;}


        }else{
            if (t.string_input.size()*sizeof(QChar)> 8)
            {t.string_input = t.previous_string_input; exceeded = true;}
        }
        if (exceeded){
            display_error("Input string exceeded 8 bytes lenght");

            qDebug() << "Input string exceeded 8 bytes lenght";
        }
        return;
    }

    t.qstring_list = t.string_input.split(settings_win->input_separator);
    t.qstring_list_size =  t.qstring_list.size();

    if (ui->dec_button->isChecked())
    {
        bool ok;
        base = 10;
        int64_t byte_dec_max =1;
        for (int h = 0; h < t.dec_bit_depth;h++)
            byte_dec_max *= 2;
        byte_dec_max-=1;
        int64_t byte_dec_max_neg = byte_dec_max*-1;

        QString byte_dec_max_s = QString::number(byte_dec_max);
        QString byte_dec_max_s_neg = "-1";//QString::number(byte_dec_max_neg);

        for (int h = 0; h < t.qstring_list_size; h++){
            int64_t n = t.qstring_list[h].toLongLong(&ok, 10);
            if (n> byte_dec_max && n > 0)
            {
                t.qstring_list[h] = byte_dec_max_s;
                reformatted.append(byte_dec_max_s);
            }
            else if ( n < byte_dec_max_neg && n < 0){
                t.qstring_list[h] = byte_dec_max_s;
                reformatted.append(byte_dec_max_s_neg);
            }else
            {
                reformatted.append( t.qstring_list[h]);
            }
            if (h != t.qstring_list_size-1)
                reformatted.append(settings_win->input_separator);

        }

    }
    else if (ui->binary_button->isChecked()){
        QString max_value_str = "";
        base = 2;
        for (int h = 0; h < t.dec_bit_depth/8; h++)
            max_value_str.append("11111111");

        for (auto &str : t.qstring_list)
        {
            int ret = str.size();
            if (ret > t.dec_bit_depth)
                str = max_value_str;
        }
        reformatted =  t.qstring_list.join(settings_win->input_separator);
    }else if (ui->hex_button->isChecked()){
        base = 16;
        return;
    }
    ui->input_box->setText(reformatted);

}


void MainWindow::update_box_bytes(){
    qDebug() << "update_box_bytes";
    ui->BE_button->setEnabled(true);
    ui->LE_button->setEnabled(true);
    ui->input_option_menu->setCurrentIndex(-1);
    ui->input_option_menu->clear();
    ui->input_option_menu->addItems(t.bit_depth_list);
    ui->input_option_menu->setCurrentIndex(0);
}

void MainWindow::on_input_box_textChanged(const QString &arg1){  update_table("on_input_box_textChanged");}
void MainWindow::on_BE_button_clicked(bool checked){    update_table("on_BE_button_clicked");}
void MainWindow::on_LE_button_clicked(){ update_table("on_LE_button_clicked"); }
void MainWindow::on_dec_button_clicked(){  update_box_bytes(); clear_input();    qDebug() << "on_dec_button_clicked ended";  }//update_table("on_dec_button_clicked");
void MainWindow::on_hex_button_clicked(bool checked){ update_box_bytes(); clear_input(); qDebug() << "on_hex_button_clicked ended";} //update_table("on_hex_button_clicked");
void MainWindow::on_binary_button_clicked(bool checked){  update_box_bytes(); clear_input(); ;}//update_table("on_binary_button_clicked")
void MainWindow::on_text_button_clicked(bool checked){
    qDebug() << "on_text_button_clicked";
    ui->BE_button->setDisabled(true);
    ui->LE_button->setDisabled(true);

    //ui->input_option_menu->setCurrentIndex(-1);
    ui->input_option_menu->clear();
    ui->input_option_menu->addItems(t.encoding_list);
    //ui->input_option_menu->setCurrentIndex(0);

    this->t.input_encoding = ui->input_option_menu->itemText(ui->input_option_menu->currentIndex());
    clear_input();

    qDebug() << "on_text_button_clicked Current input_encoding " << t.input_encoding;
}


void MainWindow::on_encoding_menu_currentIndexChanged(int index){  update_table("on_encoding_menu_currentIndexChanged");}
void MainWindow::on_buffer_preview_menu_currentIndexChanged(int index){    update_table("on_buffer_preview_menu_currentIndexChanged");}
void MainWindow::on_input_option_menu_activated(int index){ update_table("on_input_option_menu_activated");}
void MainWindow::on_group_digits_clicked(){update_table("on_group_digits_clicked");}
void MainWindow::on_settings_button_clicked(){    settings_win->show();}

