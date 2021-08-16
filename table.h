#pragma once
#ifndef TABLE_H
#define TABLE_H
#include <utils_.h>
//#include "ui_mainwindow.h"

enum input_format
{
    hex_,    decimal_,  binary_,  text_,
};
enum endianness_
{
    little_,    big_
};
enum modes
{
    text__,    bytes_
};
class table
{
public:
    table();
    QChar * input;
    QString string_input;
    QString previous_string_input;
    QStringList qstring_list;
    QString buffer_preview;
    int buffer_preview_mode = hex_;

    int qstring_list_size;
    wstring wstring_input;
    char buffer[16];
    char inverse_buffer[16];
    QByteArray utf8_buffer;

    int input_format_ = hex_;
    int endiannes = little_;
    int dec_bit_depth = 8;
    bool group_digits = false;

    QString input_encoding = "UTF-8";
    QString separator = ".";

    QStringList encoding_list = {"UTF-8", "UTF-16 LE"};
    QStringList bit_depth_list = {"8 bit", "16 bit", "32 bit"};
    int current_mode = bytes_;
    int previous_mode = bytes_;

    wstring tmp_wstr;
    QString get_variants();

    int8_t int8;
    uint8_t uint8;
    int16_t int16;
    uint16_t uint16;
    int32_t int32;
    uint32_t uint32;
    int64_t int64;
    uint64_t uint64;
    QString binary;
    QString hex;
    QString octal;
    float float_;
    double double_;
    QString utf8;
    QString utf16;


};

QStringList split_every(QString qs, int every);
int check_input(QStringList &qstring_list, int base);

#endif // TABLE_H
