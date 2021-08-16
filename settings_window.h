#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QDialog>
#include <QLineEdit>

#include <utils_.h>
namespace Ui {
class settings_window;
}

class settings_window : public QDialog
{
    Q_OBJECT

public:

    QString input_separator = " ", output_separator = " ";
    int enable_dark_theme = 0;
    QString file_path = QDir::currentPath() + "\\" + "settings.conf";
    void read_conf_file();
    int write_conf_file(bool write_defaults);

    explicit settings_window(QWidget *parent = nullptr);
    ~settings_window();

private slots:

    void on_accept_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::settings_window *ui;
};

#endif // SETTINGS_WINDOW_H
