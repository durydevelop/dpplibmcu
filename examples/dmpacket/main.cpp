#include <iostream>
#include "BotCommands.h"
#include "DMPacket.h"
#include <boost/asio.hpp>

using namespace std;

int Send(boost::asio::ip::tcp::socket &Socket, DMPacket &Packet) {
    cout << "CMD=" << to_string(Packet.GetCmd()) << " DATA=" << Packet.GetDataAsString() << endl;
    boost::system::error_code ec;
    int BytesSent=boost::asio::write(Socket,boost::asio::buffer(Packet.Get(),Packet.Size()),ec);
    if (ec) {
        cout << "ERROR=" << ec.message() << endl;
    }
    return BytesSent;
}

int Recv(boost::asio::ip::tcp::socket &Socket, DMPacket &Packet) {
    cout << "waiting for data" << endl;
    uint8_t Buffer[DMPacket::MAX_PACKET_SIZE];
    boost::system::error_code ec;
    //int BytesRecv=boost::asio::read(Socket,boost::asio::buffer(Buffer),ec);
    int BytesRecv=Socket.read_some(boost::asio::buffer(Buffer),ec);
    if (ec) {
        cout << "ERROR=" << ec.message() << endl;
        return(BytesRecv);
    }

    Packet.Set(Buffer,BytesRecv);
    std::cout << "Reply: " << "CMD=" << to_string(Packet.GetCmd()) << " DATA=" << Packet.GetDataAsString() << endl;
    return(BytesRecv);
}

int main()
{
    DMPacket Packet;

    Packet.Set(CMD_TEST_WORD,DMPacket::DATA_TYPE_WORD);
    cout << "Test WORD -> 1020, 3040" << endl;
    Packet.AddWord(1020);
    Packet.AddWord(3040);
    cout << "Count=" << to_string(Packet.GetDataCount()) << endl;
    for (uint8_t ixD=0; ixD<Packet.GetDataCount(); ixD++) {
        uint16_t word=Packet.ReadWord(ixD);
        cout << to_string(word) << endl;
    }

    Packet.Set(CMD_TEST_DWORD,DMPacket::DATA_TYPE_DWORD);
    cout << "Test DWORD -> 1001020, 1003040" << endl;
    Packet.AddDWord(1001020);
    Packet.AddDWord(1003040);
    cout << "Count=" << to_string(Packet.GetDataCount()) << endl;
    for (uint8_t ixD=0; ixD<Packet.GetDataCount(); ixD++) {
        uint32_t dword=Packet.ReadDWord(ixD);
        cout << to_string(dword) << endl;
    }

    Packet.Set(CMD_TEST_INT,DMPacket::DATA_TYPE_WORD);
    cout << "Test INT -> -5060, -7080" << endl;
    Packet.AddInt(-5060);
    Packet.AddInt(-7080);
    cout << "Count=" << to_string(Packet.GetDataCount()) << endl;
    for (uint8_t ixD=0; ixD<Packet.GetDataCount(); ixD++) {
        int Int=Packet.ReadInt16(ixD);
        cout << to_string(Int) << endl;
    }

    Packet.Set(CMD_TEST_FLOAT,DMPacket::DATA_TYPE_DWORD);
    cout << "Test FLOAT -> 10.20, 30.40" << endl;
    Packet.AddFloat(10.20f);
    Packet.AddFloat(30.40f);
    cout << "Count=" << to_string(Packet.GetDataCount()) << endl;
    for (uint8_t ixD=0; ixD<Packet.GetDataCount(); ixD++) {
        float Float=Packet.ReadFloat(ixD);
        cout << to_string(Float) << endl;
    }

/*
    try {
        DMComm dmcomm;
        if (argc != 3) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        boost::asio::ip::tcp::socket Socket(io_context);
        boost::asio::ip::tcp::resolver Resolver(io_context);
        boost::asio::connect(Socket,Resolver.resolve(argv[1], argv[2]));
        DMPacket dmPacket;
        Recv(Socket,dmPacket);

        std::cout << "Send User" << endl;
        dmPacket=dmcomm.BuildPacket(CMD_LOGON_USER,"Pilot");
        Send(Socket,dmPacket);
        Recv(Socket,dmPacket);

        std::cout << "Send Password" << endl;
        dmPacket=dmcomm.BuildPacket(CMD_LOGON_PASSWORD,"Pilot1");
        Send(Socket,dmPacket);
        Recv(Socket,dmPacket);

            std::cout << "Req Version" << endl;
            dmPacket=dmcomm.BuildPacket(CMD_GET_VERSION);
            Send(Socket,dmPacket);
            Recv(Socket,dmPacket);

			std::cout << "CMD_TEST_WORD" << endl;
			dmPacket.Set(CMD_TEST_INT,DMPacket::DATA_TYPE_WORD);

			dmPacket.AddInt(1020);
			dmPacket.AddInt(3040);
			dmPacket.AddInt(-5060);
			dmPacket.AddInt(-7080);

			//dmPacket=dmcomm.BuildPacket(CMD_TEST_WORD,DMPacket::DATA_TYPE_WORD,8,wData);
			Send(Socket,dmPacket);
    }catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
*/
    return 0;
}
