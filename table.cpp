#include "table.h"
#include <stdio.h>
#include <stdlib.h>
table::table()
{

}

QStringList split_every(QString qs, int every){
    QStringList splitted;
    QString tmp;
    for (int h = 1; h < qs.size()+1; h++){
        tmp.push_back(qs[h-1]);
        if (h%every == 0)
        {splitted.push_back(tmp);
            tmp.clear();}
    }
    for (int h = 0; h < splitted.size(); h++){
        qDebug() << h << splitted[h];
    }
    return splitted;
}

int get_number_bit_depth(int64_t n, int default_bit_depth, int mode){
    if (mode == hex_)
        return default_bit_depth;

    if (n> 0){
        if (n < 256)
            return 8;
        else if (n < 65536)
            return 16;
        else
            return 32;
    }
    else if  (n < 0)
    {
        return default_bit_depth;
        if (n > -256)
            return 8;
        else if (n > -65536)
            return 16;
        else return 32;
    }
    else return 8;
}

int check_input(QStringList &qstring_list, int base){
    bool ok;
    int size = qstring_list.size();
    if (qstring_list.size() == 1 && qstring_list[0] == "")
        return 0;
    for (auto &str : qstring_list)
    {
        auto ret = str.toLongLong(&ok, base);
        if (!ok){
            qDebug() << "size " << QString::number(size) << " invalid input " << str << " ret " << QString::number(ret) << base << " " << ok;
            return -1;
        }
    }
    return 0;

}

QString table::get_variants(){
    QStringList qstring_list;
    int qstring_list_size;
    int base = 0;
    std::vector<int64_t> byte_size_list, int_list;

    int needed_size;

    qDebug() << "string_input " << this->string_input;

    char * ptr, *preview_ptr;
    memset(buffer, 0, 16);
    memset(inverse_buffer, 0, 16);
    buffer_preview.clear();


    if (input_format_ == text_){
        int size;
        if (input_encoding == "UTF-8")
        {
            size = utf8_buffer.size();
            memcpy(buffer, utf8_buffer.constData(), size);
            for (int v = 0; v < size; v++)
                inverse_buffer[size-v-1] = buffer[v];
           // memcpy(inverse_buffer, utf8_buffer.constData(), size);
        }
        else
        {
            size =  string_input.size();
            memcpy(buffer, string_input.data(), size*sizeof(QChar));
          //  memcpy(inverse_buffer, string_input.data(), size*sizeof(QChar));
            int char_size = size*sizeof(QChar)-1;
            int buf_pos = 0, inv_buf_pos = 0;
            for (int v = 0; v <= char_size; v++)
                inverse_buffer[char_size-inv_buf_pos++] = buffer[buf_pos++];
        }
        ptr = inverse_buffer;
        preview_ptr =buffer;// buffer;

    }else{

        if (string_input.size())
        {
            while (string_input.size()){
                auto ret = string_input.right(separator.size());
                if (ret == '-' || ret == separator)
                    string_input.chop(ret.size());
                else break;
            }

        }

        switch (this->input_format_)
        {
        case hex_:
            needed_size = dec_bit_depth/4;
            while (string_input.size() %needed_size != 0)
                string_input = "0" + string_input;

            qstring_list = split_every(string_input, needed_size);

            base = 16;

            if (string_input.size() > 16){
                qDebug() << "invalid hex input, don't exceed 16 characters";
                return "Invalid hex input, don't exceed 16 characters";
            }

            break;
        case decimal_:
            qstring_list = this->string_input.split(separator);
            qDebug() << "decimal_ " << string_input;
            base = 10;

            break;

        case binary_:
            qstring_list = this->string_input.split(separator);
            base = 2;
//            if (string_input.size() > 64+dec_bit_depth*separator.size()*sizeof(QChar)){
//                qDebug() << "invalid binary input, don't exceed 64 characters";
//                return "invalid binary input, don't exceed 64 characters";
//            }

            break;
        }
        if (check_input(qstring_list, base) == -1)
            return QString("Invalid base " + QString::number(base, 10) + " input");


        qstring_list_size = qstring_list.size();

        int buffer_position = 0;
        int inverse_buffer_position = 0;
        int block_size = this->dec_bit_depth / 8;
        int total_input_byte_size = 0;


        byte_size_list.clear();
        int_list.clear();

        for (int n = 0; n < qstring_list_size; n++)
        {
            bool ok;
            int_list.push_back(qstring_list[n].toLongLong(&ok, base));
            byte_size_list.push_back(get_number_bit_depth(int_list[n], this->dec_bit_depth, input_format_)/8);
            inverse_buffer_position+=byte_size_list[n];
            total_input_byte_size += byte_size_list[n];
            if (total_input_byte_size > 8)
                return "Input exceeded 8 byte maximum size";
        }

        for (int n = 0; n < qstring_list_size; n++)
        {
            block_size = byte_size_list[n];

            memcpy(buffer+buffer_position ,&int_list[n], block_size);
            memcpy(inverse_buffer+inverse_buffer_position-block_size ,&int_list[n], block_size);

            buffer_position  +=  block_size;
            inverse_buffer_position  -=  block_size;
        }
        if (this->endiannes == big_)
            ptr = inverse_buffer;
        else
            ptr = buffer;
        preview_ptr = ptr;
    }



    memcpy(&uint64, ptr, 8);

    if (buffer_preview_mode == hex_)
        base = 16;
    else base = 10;
    for (int h = 0; h < 8; h++)
        buffer_preview.append(QString::number((uint8_t)preview_ptr[h], base).leftJustified(4, ' '));
    //buffer_preview = QString("%1%2%3").arg((uint8_t)ptr[0], -3).arg((uint8_t)ptr[1], -3).arg((uint8_t)ptr[2], -3);


    qDebug() << "**";
    for (int h = 0; h < 8; h++)
        qDebug() << h << " nor " << (int)buffer[h] << " inv " << (int)inverse_buffer[h];

    this->uint32 = this->uint64;
    this->uint16 = this->uint64;
    this->uint8 = this->uint64;

    this->int32 = this->uint64;
    this->int16 = this->uint64;
    this->int8 = this->uint64;
    this->int64 = this->uint64;

    this->binary = QString::number(uint64, 2);
    this->hex =  QString::number(uint64, 16);
    this->octal =  QString::number(uint64, 8);

    this->double_ = *reinterpret_cast<double*>(&uint64);
    this->float_ = *reinterpret_cast<float*>(&uint32);

    if (this->endiannes == big_)
    {
        this->utf8 = QString::fromUtf8((char*)buffer);
        this->utf16 = QString::fromUtf16((char16_t*)buffer);
    }
    else
    {
        this->utf8 = QString::fromUtf8((char*)inverse_buffer);
        this->utf16 = QString::fromUtf16((char16_t*)inverse_buffer);

    }
    return "";
}
