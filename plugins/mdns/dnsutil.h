/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef DNSUTIL_H
#define DNSUTIL_H

#include <QByteArray>
#include <QMap>
#include <QtEndian>

#include "dnsmessage.h"

/**
 * @brief Utility methods for working with DNS data
 */
class DnsUtil
{
public:

    /**
     * @brief Parse a raw DNS packet into a DnsMessage
     * @param packet raw DNS packet
     * @param message DnsMessage
     * @return true if no errors occurred
     */
    static bool fromPacket(const QByteArray &packet, DnsMessage &message);

    /**
     * @brief Create a raw DNS packet from a DnsMessage
     * @param message DnsMessage
     * @param packet raw DNS packet
     */
    static void toPacket(const DnsMessage &message, QByteArray &packet);

    /**
     * @brief Read a DNS name
     * @param packet raw DNS packet
     * @param offset offset into packet
     * @param name value read from packet
     * @return true if no errors occurred
     */
    static bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name);

    /**
     * @brief Write a DNS name
     * @param packet raw DNS packet
     * @param offset offset into packet
     * @param name value to write to packet
     * @param nameMap map of DNS names already written to their offsets
     */
    static void writeName(const QByteArray &packet, quint16 &offset, const QByteArray &name, QMap<QByteArray, quint16> nameMap);

    /**
     * @brief Read an integer in network byte-order from a byte array
     * @param packet raw DNS packet
     * @param offset offset into packet
     * @param value integer to read
     * @return true if the integer was read
     */
    template<class T>
    static bool parseInteger(const QByteArray &packet, quint16 &offset, T &value)
    {
        if (offset + sizeof(T) > packet.length()) {
            return false;  // out-of-bounds
        }
        value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(packet.constData() + offset));
        offset += sizeof(T);
        return true;
    }

    /**
     * @brief Write an integer in network byte-order to a byte array
     * @param packet raw DNS packet
     * @param offset offset into packet
     * @param value integer to write
     */
    template<class T>
    static void writeInteger(QByteArray &packet, quint16 &offset, T value)
    {
        value = qToBigEndian<T>(value);
        packet.append(reinterpret_cast<const char*>(&value), sizeof(T));
        offset += sizeof(T);
    }
};

#endif // DNSUTIL_H