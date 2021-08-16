#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSlider>
#include <QMainWindow>
#include <table.h>
#include <settings_window.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    table t;
    settings_window *settings_win;


private slots:
    void on_input_box_textChanged(const QString &arg1);

    void update_table(QString from);

    void check_string();

    int get_input_format();

    void display_error(QString error_msg);
    void clear_input();

    bool eventFilter(QObject *obj, QEvent *event);

    QString group_digits(QString input, int divider);
    //void on_endian_slider_sliderMoved(int position);

    void on_BE_button_clicked(bool checked);


    void on_text_button_clicked(bool checked);


    void on_binary_button_clicked(bool checked);

    //void on_dec_button_clicked(bool checked);

    void on_hex_button_clicked(bool checked);

   // void on_encoding_menu_currentIndexChanged(const QString &arg1);

    void on_encoding_menu_currentIndexChanged(int index);

    //void on_input_option_menu_currentIndexChanged(int index);
    void update_box_bytes();


    void on_dec_button_clicked();

    void on_buffer_preview_menu_currentIndexChanged(int index);

    void on_LE_button_clicked();


    void on_input_option_menu_activated(int index);

    void on_group_digits_clicked();

    void on_settings_button_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
