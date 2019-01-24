#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h> //fork sleep
#include <vector>
#include <map>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <algorithm>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <string>
//#include <ctime>
#define buf 2048

using namespace std;
#define inf (INT_MAX)
#define max_routers 6
class router
{
	bool validity;	//whether valid or not
	int nexthop_ports;	//next port number
	char nexthop_names;	//name of next router
	int costs;	//distance cost to next router

public: 
	int nexthop_port()  //check if valid, send port number
	{
		if(invalid())
		return inf;
		else return nexthop_ports;
	}
	char nexthop_name() //check if valid, send router name
	{
		if(invalid())
			return '0';
		else return nexthop_names;
	}
	int cost() //checks if valid, sends cost to router
	{
		if(invalid())
		return inf;
		else return costs;
	}
	bool invalid(){ return validity;}	//invalid boolean 
	void setvalidity(){validity=false;}	//set to valid
	void setinvalidity(){validity=true;}	//set to invalid
	void setnexthop_port(int n){nexthop_ports=n;}	//set next port as n
	void setnexthop_name(char n){nexthop_names=n;}	//set next name as n 
	void setcost(int c){costs=c;}	//set cost as c 
};
class r_node
{
public:
	char name;
	int port;
	timespec start_time;
	sockaddr_in addr;
};
class DistVector{
	private:
		int dv_self;	//index of self
		router dv_entries[max_routers];
		router initial_entries[max_routers];
		vector<r_node> dv_neighbours;	//list of neighbouring nodes/routers
		sockaddr_in dv_myaddr; 				//Why here?
		map<char, int> dv_portno;

		void print(router R[], char name, string msg, bool curr_time)
		{
			cout << msg << ": " << name << endl;
			if (curr_time)
			{
				time_t rawtime;			//declare time var
				time(&rawtime);			//store current time
				cout << ctime(&rawtime);//convert time_t to string and 
								//print in time formmat
			}
			cout << "|Destination|   Cost   | Outgoing_UDP |    Destination Port \n";
			for (int dest = 0; dest < max_routers; dest++)
			{
				if(R[dest].cost()==inf)
					{continue;}
				cout << "|     " << nameOf(dest) << "     | ";
				if (R[dest].cost() != inf)
					cout << "    ";
				cout << R[dest].cost()<<"    |";
				if (R[dest].nexthop_port() == inf)
					cout << "         |   ";
				cout <<portNoOf(nameOf(indexOf(name)))<<" Node("<<name<<")       "<< portNoOf(nameOf(dest))<<" Node("<<nameOf(dest)<<")\n";
			}
			cout << endl;			
		}
		int calc_minDist(int original, int self_Intermediate, int intermediate_Destination, char originalName, char newName, bool &updated)
		{
			int newCost = self_Intermediate + intermediate_Destination;
			if(self_Intermediate == inf || intermediate_Destination == inf)
				return original;		//No link; Skip
			else if( original == inf)
			{
				updated = true;
				return newCost;			//found new link
			}
			else if (newCost < original)
			{
				updated = true;
			//	newName = originalName;
				return newCost;			//found new least cost path
			}
			else if (original == newCost)	//cost same
			{
				if (originalName <= newName)	//order by name
				{
					updated = false;
				//	newName = originalName;
				}	
				else
				{
					updated = true;
					//newName = originalName;
					return newCost;
				}
				return original;
			}
			else
				return original;			//no updation. Continue
		}
	public:
		router *getRouter() {return dv_entries; }
		int getSize() { return sizeof(dv_entries);}
		char getName() {return nameOf(dv_self);}
		router routeTo(char dest) {return dv_entries[indexOf(dest)]; }
		vector<r_node> neighbours() { return dv_neighbours; }
		sockaddr_in myaddr() { return dv_myaddr; }
		int port() { return portNoOf(getName()); }
		int portNoOf(char router){	return dv_portno[router];}
		char nameOf(int index){	return (char)index + 'A';}
		int indexOf(char router){	return router - 'A';}
				
		//fn definations
		DistVector(char *fil, char *self)
		{
			fstream file(fil);

			string line,field;//current line of the file and the current token
						  //that is to be put into the file
			char selfname=self[0];
			dv_self = indexOf(self[0]);

			//initialize the entries
			for(int dest=0; dest<max_routers; dest++)
			{
				dv_entries[dest].setnexthop_name('0');
				dv_entries[dest].setnexthop_port(inf);
				dv_entries[dest].setcost(inf);
				dv_entries[dest].setvalidity();
			}
			while(getline(file,line))// parsing file line by line
			{
				stringstream lining(line);
				router entry;
				entry.setvalidity();
		
				getline(lining,field,',');//source router
				char src = field[0];
		
				getline(lining,field,',');//destination router
				int dest = indexOf(field[0]);
				r_node n; n.name=field[0];
				entry.setnexthop_name(field[0]);
		
				getline(lining,field,',');
				int port=atoi(field.c_str());
				entry.setnexthop_port(port);
				n.port=port;
				//////////////////////
				memset((char *)&n.addr,0,sizeof(n.addr));
				n.addr.sin_family = AF_INET;//At family internet links socket to internet
				n.addr.sin_addr.s_addr = inet_addr("127.0.0.1");//sending internet address
				n.addr.sin_port = htons(port);//socket readable port no.
				getline(lining,field,',');//cost of the router
				entry.setcost(atoi(field.c_str()));
			    int j=0;
				//for(int i = 0; i < dv_neighbours.size(); i++)
		 		//	cout<<"Khach"<<dv_neighbours[i].name<<endl;
				if(selfname == 'H')
				{
		 			int i;
		 			for(i = 0; i < dv_neighbours.size(); i++)
		 			if(dv_neighbours[i].name==n.name)
			 			 break;
		 			if(i == dv_neighbours.size())
			 			dv_neighbours.push_back(n);	
				}
				else if(src == selfname)
				{
					startTimer(n);
					dv_neighbours.push_back(n);//store neighbours
					dv_entries[dest]=entry;
				}
				dv_portno[n.name] = n.port;
			}
			dv_portno['H'] = 10101;//special port for sending data packet
			memcpy((void*)initial_entries,(void*)dv_entries,sizeof(dv_entries));
			if(nameOf(dv_self)!='H')
			print(dv_entries,nameOf(dv_self),"INITIAL ROUTING TABLE", true);
			//////////////////////
		}
		void reset(char dead)
		{
			for (int i = 0; i < dv_neighbours.size(); i++)
			{
				if (dv_neighbours[i].name == dead)
				{
					if (initial_entries[indexOf(dead)].cost() != inf)
						initial_entries[indexOf(dead)].setinvalidity();
				}
			}
			memcpy((void*)dv_entries, (void*)initial_entries, sizeof(dv_entries));
			print(dv_entries, nameOf(dv_self), "RESET ROUTING TABLE", true);
		}
		bool update(void *adbuf, char src)
		{
			router original_entry[max_routers];
			memcpy((void*)original_entry, (void*)dv_entries,sizeof(dv_entries));

			bool updated = false;
			int intermediate = indexOf(src);
			if (initial_entries[intermediate].invalid())
			{
				initial_entries[intermediate].setvalidity();
				dv_entries[intermediate].setvalidity();
				updated = true;
			}
			router ad[max_routers];//loading advertised distance vector
			memcpy((void*)ad,adbuf,sizeof(ad)); 
			//recalculating self's distance vector
			for(int dest=0; dest<max_routers;dest++)
			{
				if(dest==dv_self)
					continue;
				bool updated_entry=false;
				dv_entries[dest].setcost(calc_minDist(dv_entries[dest].cost(), dv_entries[intermediate].cost(), ad[dest].cost(), dv_entries[dest].nexthop_name(), src, updated_entry));
				if (updated_entry)
				{
					updated = true;
					dv_entries[dest].setnexthop_port(portNoOf(src));
					dv_entries[dest].setnexthop_name(src);
				}
			}
			dv_entries[intermediate].setcost(ad[dv_self].cost());

			if (updated)
			{
				print(original_entry, nameOf(dv_self), "CHANGE DETECTED!\nROUTING TABLE BEFORE CHANGE", true);
				print(ad, src, "DV THAT CAUSED THE CHANGE", false);
				print(dv_entries, nameOf(dv_self), "ROUTING TABLE AFTER CHANGE", false);
			}
			return updated;
		}
		void init_addr(int portno)
		{
			memset((char *)&dv_myaddr, 0, sizeof(dv_myaddr)); 	//set 0 to myaddr
			dv_myaddr.sin_family = AF_INET;						//Internet family
			dv_myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//fixed IP
			dv_myaddr.sin_port = htons(portno);					//machine understandable port no
		}		
		void startTimer(r_node &n)
		{
			clock_gettime(CLOCK_MONOTONIC, &n.start_time);
		}
		bool timeExpired(r_node &n)
		{
			timespec tend={0,0};					//broken into seconds and nanoseconds
			clock_gettime(CLOCK_MONOTONIC, &tend);

			if (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)n.start_time.tv_sec + 1.0e-9*n.start_time.tv_nsec) > 5)
				return true;		//5 second time limit (given)
			else
				return false;
		}
		void printfn(router R[])
		{
			int i=20;
			for (int dest = 0; dest < max_routers; dest++)
			{
				if(R[dest].cost()==inf)
					{i--;continue;}
			}
			if(i==19)
			{	
				time_t rawtime;			//declare time var
				time(&rawtime);			//store current time
										//convert time_t to string and 
								//print in time formmat
				cout<<"\nStable State Reached All nodes converged. Time of convergence:"<<ctime(&rawtime);
				cout<<"\nStable State Reached All nodes converged\n Destination: ";
				
				for (int dest = 0; dest < max_routers; dest++) 
					cout<<nameOf(dest)<<"  ";
				cout<<"\n Cost:        ";
				for (int dest = 0; dest < max_routers; dest++)
				{
					if(R[dest].cost() == inf)
						cout<<"0  ";
					else
					cout<<R[dest].cost()<<"  ";
				}
				cout<<endl;
				/*

				for (int dest = 0; dest < max_routers; dest++) 
					if (R[dest].cost()!=inf) cout<<"    "<<nameOf(dest)<<"            "<<R[dest].cost()<<" \n";
					else cout<<"    "<<nameOf(dest)<<"            0 \n";
				cout<<endl;	
				*/
			}
		}
};
//HELPER FUNCTIONS
//~~~~~~~~~~~~~~~~
class header
{
public:
	int type,len;
	char src,dest;
};
enum type
{
	TYPE_DATA, TYPE_ADVERTISEMENT, TYPE_WAKEUP, TYPE_RESET
};
void *create_packet(int type, char src, char dest, int payload_length, void *payload)
{
	int allocated_payload_length = payload_length;
	if((type != TYPE_DATA) && (type != TYPE_ADVERTISEMENT))//Either the router was 
	allocated_payload_length = 0;//idle or getting valid fron invalid or invalid
	void *packet = operator new (sizeof(header)+ allocated_payload_length); //create empty packet
	header h;
	h.type = type;
	h.src = src;
	h.dest = dest;
	h.len = payload_length;
	memcpy(packet, (void*)&h, sizeof(header));
	//cout<<"\nPacket "<<packet<<"\nHeader "<<sizeof(header)<<payload<<endl;
	memcpy((void*)((char*)packet+sizeof(header)), payload, allocated_payload_length);
	return packet;
}
header get_header(void *packet)
{
	header h;
	memcpy((void*)&h,packet, sizeof(header));
	return h;
}
void *get_payload(void *packet, int len)
{
	void *payload = operator new (len);
	memcpy(payload, (void*)((char*)packet+sizeof(header)),len);
	return payload;
}
void multi_cast(DistVector &dv, int socket)
{
	vector<r_node> neighbours = dv.neighbours();
	//counter = 0;
	for( int i=0; i<neighbours.size();i++)
	{
		void *send_packet = create_packet(TYPE_ADVERTISEMENT, dv.getName(), neighbours[i].name, dv.getSize(), (void*)dv.getRouter());
		sendto(socket, send_packet, sizeof(header)+dv.getSize(),0,(  sockaddr *)&neighbours[i].addr, sizeof(sockaddr_in));
		free(send_packet);
	}
}
void self_cast(DistVector &dv, int socketfd,int type, char src=0, char dest=0, int payload_length=0, void *payload=0)
{
	void *send_packet = create_packet(type,src,dest,payload_length,payload);
	sockaddr_in dest_addr = dv.myaddr();
	sendto(socketfd, send_packet,sizeof(header),0,(sockaddr *)&dest_addr,sizeof(sockaddr_in));
	free(send_packet);
}

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		cerr<<"Not enough arguments.\nUsage: ./my-router <initialization file> <router name>\n";
		return 0;
	}
	DistVector dv(argv[1],argv[2]);
	vector<r_node> neighbours = dv.neighbours();
	int my_port = dv.portNoOf(argv[2][0]);
	dv.init_addr(my_port);
	sockaddr_in myaddr = dv.myaddr();

	socklen_t addr_len = sizeof(sockaddr_in); //socket length_type addr_len storing length of addresses
	int socketfd;// create a UDP socket
	if((socketfd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
	{
		cerr<<"cannot create socket\n";
		return 0;
	}
	if (bind(socketfd, (  sockaddr *)&myaddr, sizeof(myaddr)) < 0)// bind the socket to localhost and myPort
	{
		cerr<<"Bind failed";
		return 0;
	}
	if (dv.getName() == 'H')//send a data packet to router A
	{
		char data[100];
		memset(data,0,100);
		cin.getline(data, 100);
		for(int i=0; i<neighbours.size(); i++)
			if(neighbours[i].name=='A')
			{
				void *data_packet = create_packet(TYPE_DATA, dv.getName(), 'D', strlen(data), (void*)data);
				sendto( socketfd, data_packet, sizeof(header)+dv.getSize(), 0, (sockaddr *)&neighbours[i].addr, sizeof(sockaddr_in));//socketfd ki values neighbours.i mein bhejta hai 0 is flag
				
				header h = get_header(data_packet);
				cout<<"Sent data packet\nType data\nSource: "<<h.src<<"\nDestination: "<<h.dest
					<<"\nLength of Packet: "<<sizeof(header)+h.len<<"\nLength of Payload: "<<h.len<<"\nPayload: "<<data;
				free(data_packet);cout<<endl;
			}
		exit(0);
	}
	//int flag = 0;
	int counter = 0;
	int pid = fork();//distance vector routing
					//In case one of the two tasks fail the other one is not affected also so that both the processes can run simutaneouly
	if(pid < 0)
	{
		cerr<<"Fork failed";
		return 0;
	}
	else if(pid == 0)// send to each neighbor periodically
		while(1)
		{
			self_cast(dv, socketfd, TYPE_WAKEUP);
			sleep(5);
		}
	else //listen for advertisements
	{
		void *rcvbuf = operator new (buf);
		sockaddr_in rem_addr;
		while(1)
		{
			memset(rcvbuf, 0, buf);
			int recv_len = recvfrom(socketfd, rcvbuf, buf, 0, (sockaddr*)&rem_addr, &addr_len);

			header h = get_header(rcvbuf);
			void *payload = get_payload(rcvbuf, h.len);
			switch(h.type)
			{
				case TYPE_DATA:
					//counter=0;
					cout<< "Received Data Packet! \n";
					time_t rawtime;
					time(&rawtime);
					cout<<"Current Time: "<< ctime(&rawtime)<<"\nSource node ID: "<< h.src << endl<<"Destination ID: "<< h.dest << endl<<"UDP port in which packet arrived: "<< my_port<<endl;
					if(h.dest != dv.getName()) //FORWARD ONLY WHEN ROUTER IS NOT THE DESTINATION
					{
						if(dv.routeTo(h.dest).nexthop_port()== inf)
							cerr<<"ERROR: Packet forwarding failed. \n";
						else
						{
							cout<<"Packet forwarded through UDP port: "<< dv.routeTo(h.dest).nexthop_port()<<"\nPacket forwarded to node ID: "<< dv.routeTo(h.dest).nexthop_name() << endl;
							void *forwardPacket = create_packet(TYPE_DATA, h.src, h.dest, h.len, (void*)payload);
							//void *forwardPacket = create_packet(TYPE_DATA, dv.nameOf(my_port-10000), h.dest, h.len, (void*)payload);
							for (int i = neighbours.size()  ; i >= 0; i--)
								if(neighbours[i].name == dv.routeTo(h.dest).nexthop_name()) //&& dv.getRouter()[dv.indexOf(h.dest)].cost()==minimum(dv.getRouter()))//|| neighbours[i].name =='F' )
									sendto(socketfd, forwardPacket, sizeof(header) + dv.getSize(), 0, (sockaddr *)&neighbours[i].addr, sizeof(sockaddr_in));
							free(forwardPacket);
						}
						cout<< endl;
					}
					else //DESTINATION ROUTER; EXTRACT DATA
					{
						char data[100];
						memset(data, 0, 100);
						memcpy((void*)data, payload, h.len);
						cout << "Data payload: " << data << endl << endl;
					}
					break;
				case TYPE_ADVERTISEMENT: //ADVERTISE ITS' ROUTER
					//router entries[max_routers];
					//memcpy((void*)entries, payload, h.len);
					//counter--;
					for(int i = 0; i < neighbours.size(); i++)
						if(neighbours[i].name == h.src)
							dv.startTimer(neighbours[i]);
					//flag=0;
					if(dv.update(payload, h.src)==true) counter=0;
					break;
				case TYPE_WAKEUP: //PERIODIC WAKEUP CALLS 
					counter++;
					//router *R = getRouter();
					if (counter==5)
					{
						dv.printfn(dv.getRouter());
					 	//counter = 0; flag=1;
					}	
					for(int i = 0; i< neighbours.size(); i++)
					{
						 r_node curr_neighbour = neighbours[i];
						 if((dv.getRouter()[dv.indexOf(curr_neighbour.name)].cost()!= inf) && dv.timeExpired(neighbours[i]))
						 	self_cast(dv, socketfd, TYPE_RESET, dv.getName(), neighbours[i].name, dv.getSize() / sizeof(router) - 2);
					}
					multi_cast(dv, socketfd);
					break;
				case TYPE_RESET: //RESET ROUTER TABLE ??
					//counter--;//flag = 0;
					int hopcount = (int)h.len - 1;
					dv.reset(h.dest);
					if(hopcount > 0)
					{
						void *fwd_packet = create_packet(TYPE_RESET,  dv.getName(), h.dest, hopcount, (void*)0);
						for( int i = 0; i< neighbours.size(); i++)
							if (neighbours[i].name != h.src)
								sendto(socketfd, fwd_packet, sizeof(header), 0, (sockaddr *)&neighbours[i].addr, sizeof(sockaddr_in));
					}
					break;
			}
		}
		free(rcvbuf);
	}
}