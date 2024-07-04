#ifndef GDBSERVER_H
#define GDBSERVER_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "TcpServer.hpp"

class GdbServer {
  public:
    // 必须实现的方法
    virtual void continueExecution() = 0;
    virtual void readRegister(int regNo, uint32_t *value) = 0;
    virtual void readMemory(uint32_t address, uint8_t *value) = 0;
    virtual void setBreakpoint(uint32_t address) = 0;
    virtual void deleteBreakpoint(uint32_t address) = 0;

    virtual ~GdbServer() = default;

    void gdbInit(int port) {
        DEBUG("GdbServer enable at 127.0.0.1:", port);
        tcpServer.Start(port);
        tcpServer.AcceptClient();
    }
    void gdbRun() {
        running = true;
        while (running) {
            std::string strBuffer;
            strBuffer = RecvMsgBlocking();
            tcpServer.SendMsg(kMsgAck);
            DecodeAndCall(strBuffer);
        }
    }

  private:
    char const *kMsgEmpty = "$#00";
    char const *kMsgAck = "+";
    char const *kMsgOk = "$OK#9a";
    const uint32_t MaxMessageLength = 4096;
    bool running = false;
    TcpServer tcpServer;

    std::string RecvMsgBlocking() {
        std::string strBuffer("");
        std::string resp("");
        // 检测信息开头，确定信息类型
        while (true) {
            resp = tcpServer.RecvBlocking(1);
            if (resp.at(0) == '$') {
                break;
            } else if (resp.at(0) == '+') {
                continue;
            } else if (resp.at(0) == '-') {
                ERROR("received '-', protocol error");
            } else if (resp.at(0) == '\x03') {
                ERROR("received Ctrl+C!");
            } else {
                ERROR("unknow msg :", resp);
            }
        }
        // 获得完整的命令
        strBuffer.append(resp);
        do {
            resp = tcpServer.RecvBlocking(1);
            strBuffer.append(resp);
            if (strBuffer.length() > MaxMessageLength) {
                ERROR("message is too long");
            }
        } while (resp.at(0) != '#');
        std::string checkSum = tcpServer.RecvBlocking(2);
        strBuffer.append(checkSum);
        // TODO 校验 (通常来说，tcp已经包含校验了，就不再手动校验了)
        return strBuffer;
    }
    void DecodeAndCall(const std::string &msg) {
        std::string msgStripped = msg.substr(1, msg.length() - 4);
        char request = msgStripped[0];

        switch (request) {
        case 'q':
            CmdQuery(msgStripped);
            break;
        case 'g':
            CmdRegRead();
            break;
        case 'm':
            CmdReadMem(msgStripped);
            break;
        case 'Z':
            CmdSetBreakpoint(msgStripped);
            break;
        case 'z':
            CmdDeleteBreakpoint(msgStripped);
            break;
        case 'c':
            CmdContinue();
            break;
        case 'v':
            CmdVpacket(msgStripped);
            break;
        case 'D':
            running = false;
            break;
        case 'p':
            CmdRegReadNum(msgStripped);
            break;
        case '?':
            tcpServer.SendMsg(Packetify("S05").c_str());
            break;
        default:
            tcpServer.SendMsg(kMsgEmpty);
        }
    }
    std::string GetChecksumStr(const std::string &msg) {
        uint checksum = 0;
        for (const char &c : msg) {
            checksum += static_cast<uint>(c);
        }
        checksum &= 0xff;

        char buffer[5];
        sprintf(buffer, "%02x", checksum);
        return std::string(buffer);
    }
    std::string Packetify(std::string msg) {
        std::string checksum = GetChecksumStr(msg);
        msg.insert(0, "$");
        msg.append("#" + checksum);
        return msg;
    }

    // gdb指令的具体实现
    void CmdQuery(const std::string &payload) {
        std::string request = payload.substr(1, 3);

        if (request == "Sup") { // Supported
            // 启用下面这个功能，需要提供架构信息
            tcpServer.SendMsg(Packetify("PacketSize=1024;qXfer:features:read+").c_str());
            // tcpServer.SendMsg(Packetify("PacketSize=1024").c_str());
        } else if (request == "Att") { // Attached
            // 表明当前是附加到一个已经在运行的进程上
            tcpServer.SendMsg(Packetify("1").c_str());
        } else if (request == "Xfe") { // Xfer
            tcpServer.SendMsg(Packetify("l" + riscv32RegisterXML).c_str());
        } else if (request == "Sym") { // Symbol
            tcpServer.SendMsg(kMsgOk);
        } else {
            tcpServer.SendMsg(kMsgEmpty);
        }
    }
    void CmdRegRead() {
        uint32_t values[33];
        for (int i = 0; i <= 32; ++i) {
            uint32_t value = 0;
            readRegister(i, &value);
            values[i] = value;
        }
        char result[33 * 8 + 1];
        result[33 * 8] = '\0';

        uint8_t *reg = (uint8_t *)values;
        for (size_t i = 0; i < sizeof(values); i++) {
            sprintf(result + (i * 2), "%02x", reg[i]);
        }
        tcpServer.SendMsg(Packetify(result).c_str());
    }
    void CmdRegReadNum(const std::string &payload) {
        uint32_t regNum = 0, value = 0;
        sscanf(payload.substr(1).c_str(), "%x", &regNum);
        readRegister(regNum, &value);

        char result[8 + 1];
        result[8] = '\0';
        uint8_t *reg = (uint8_t *)(&value);
        for (size_t i = 0; i < sizeof(value); i++) {
            sprintf(result + (i * 2), "%02x", reg[i]);
        }
        tcpServer.SendMsg(Packetify(result).c_str());
    }
    void CmdReadMem(const std::string &payload) {
        std::string cmd = payload.substr(1);
        uint32_t addr, len;
        sscanf(cmd.c_str(), "%x,%x", &addr, &len);

        std::string msgResp("");
        for (uint32_t i = addr; len--; i++) {
            uint8_t value;
            char result[3];
            readMemory(i, &value);
            sprintf(result, "%02x", value);
            msgResp.append(result);
        }
        tcpServer.SendMsg(Packetify(msgResp).c_str());
    }
    void CmdSetBreakpoint(const std::string &payload) {
        uint32_t type, addr, kind;
        sscanf(payload.substr(1).c_str(), "%x,%x,%x", &type, &addr, &kind);
        setBreakpoint(addr);
        tcpServer.SendMsg(kMsgOk);
    }
    void CmdDeleteBreakpoint(const std::string &payload) {
        uint32_t type, addr, kind;
        sscanf(payload.substr(1).c_str(), "%x,%x,%x", &type, &addr, &kind);
        deleteBreakpoint(addr);
        tcpServer.SendMsg(kMsgOk);
    }
    void CmdContinue() {
        continueExecution();
        const int GDB_SIGNAL_TRAP = 5;
        char packetStr[32];
        sprintf(packetStr, "S%02x", GDB_SIGNAL_TRAP);
        tcpServer.SendMsg(Packetify(packetStr).c_str());
    }
    void CmdVpacket(const std::string &payload) {
        std::string request = payload.substr(1, 3);

        if (request == "Con") { // Cont?
            tcpServer.SendMsg(Packetify("vCont;c").c_str());
        } else {
            tcpServer.SendMsg(kMsgEmpty);
        }
    }

    const std::string riscv32RegisterXML = R"(<?xml version="1.0"?>
<!DOCTYPE target SYSTEM "gdb-target.dtd">
<target>
<architecture>riscv:rv32</architecture>

<feature name="org.gnu.gdb.riscv.cpu">
    <reg name="zero" bitsize="32" type="int" regnum="0"/>
    <reg name="ra" bitsize="32" type="code_ptr" regnum="1"/>
    <reg name="sp" bitsize="32" type="data_ptr" regnum="2"/>
    <reg name="gp" bitsize="32" type="data_ptr" regnum="3"/>
    <reg name="tp" bitsize="32" type="data_ptr" regnum="4"/>
    <reg name="t0" bitsize="32" type="int" regnum="5"/>
    <reg name="t1" bitsize="32" type="int" regnum="6"/>
    <reg name="t2" bitsize="32" type="int" regnum="7"/>
    <reg name="fp" bitsize="32" type="data_ptr" regnum="8"/>
    <reg name="s1" bitsize="32" type="int" regnum="9"/>
    <reg name="a0" bitsize="32" type="int" regnum="10"/>
    <reg name="a1" bitsize="32" type="int" regnum="11"/>
    <reg name="a2" bitsize="32" type="int" regnum="12"/>
    <reg name="a3" bitsize="32" type="int" regnum="13"/>
    <reg name="a4" bitsize="32" type="int" regnum="14"/>
    <reg name="a5" bitsize="32" type="int" regnum="15"/>
    <reg name="a6" bitsize="32" type="int" regnum="16"/>
    <reg name="a7" bitsize="32" type="int" regnum="17"/>
    <reg name="s2" bitsize="32" type="int" regnum="18"/>
    <reg name="s3" bitsize="32" type="int" regnum="19"/>
    <reg name="s4" bitsize="32" type="int" regnum="20"/>
    <reg name="s5" bitsize="32" type="int" regnum="21"/>
    <reg name="s6" bitsize="32" type="int" regnum="22"/>
    <reg name="s7" bitsize="32" type="int" regnum="23"/>
    <reg name="s8" bitsize="32" type="int" regnum="24"/>
    <reg name="s9" bitsize="32" type="int" regnum="25"/>
    <reg name="s10" bitsize="32" type="int" regnum="26"/>
    <reg name="s11" bitsize="32" type="int" regnum="27"/>
    <reg name="t3" bitsize="32" type="int" regnum="28"/>
    <reg name="t4" bitsize="32" type="int" regnum="29"/>
    <reg name="t5" bitsize="32" type="int" regnum="30"/>
    <reg name="t6" bitsize="32" type="int" regnum="31"/>
    <reg name="pc" bitsize="32" type="code_ptr" regnum="32"/>
</feature>

<feature name="org.gnu.gdb.riscv.virtual">
    <reg name="priv" bitsize="32" type="int" regnum="69"/>
</feature>

<feature name="org.gnu.gdb.riscv.csr">
    <reg name="mie" bitsize="32" type="int" regnum="0x304"/>
    <reg name="mtval" bitsize="32" type="int" regnum="0x343"/>
    <reg name="mstatus" bitsize="32" type="int" regnum="0x300"/>
    <reg name="mtvec" bitsize="32" type="int" regnum="0x305"/>
    <reg name="mscratch" bitsize="32" type="int" regnum="0x340"/>
    <reg name="mepc" bitsize="32" type="int" regnum="0x341"/>
    <reg name="mcause" bitsize="32" type="int" regnum="0x342"/>
    <reg name="mip" bitsize="32" type="int" regnum="0x344"/>
</feature>

</target>)";
};

#endif // GDBSERVER_H
