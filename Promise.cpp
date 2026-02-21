/*
 High-level view:
  * Can't have new thread of each new incoming task (eg data packet - Networking)
  * Have few threads, each handling multiple things
  * One thread is WAITING on a FUTURE
  * Other thread can SET the value and use PROMISE. It makes the FUTURE ready
  * TODO: I haven't understood it properly
*/

#include <future>

void ProcessConnections(ConnectionSet& connections) {
  conn_it conn;
  conn_it end = connections.end();
  // 1. Go through all the network connections
  while (!Done(connections)) {
    for (conn = connections.begin(); conn != connections.end(); ++conn) {
      // 2. Have to read data packet from a connection
      if (conn->HasIncomingData) {
        // 2a. 
        DataPacket data_packet = conn->Incoming();
        std::promise<PayloadType>& promise = conn->GetPromise(data_packet.id);
        promise.set_value(data.payload);

      } else if (conn->HasOutgoingData) {
        // 3. OR, Have to send data through a connection
        DataPacket outgoing_packet = conn->TopOfOutgoingQueue();
        conn->send(outgoing_packet.payload);
        outgoing_packet.promise.set_value(true);
      }
    }
  }
}