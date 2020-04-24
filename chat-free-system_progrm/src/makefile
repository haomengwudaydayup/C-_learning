CFLAGS=-std=c++11 -g    
LDFLAGS=-lpthread -ljsoncpp    
    
SERBIN=../bin/ser/Chatser    
CLIBIN=../bin/ser/Client                                                                                                                                                                                                                                                     
    
    
.PHONY:all    
all:$(SERBIN) $(CLIBIN)    
    
$(SERBIN):ChatServer.cpp    
    g++ $^ -o $@ $(CFLAGS) $(LDFLAGS)    
$(CLIBIN):ChatClient.cpp    
    g++ $^ -o $@ -g $(LDFLAGS) -lncursesw    
    
    
.PHONY:clean    
clean:    
    rm -f $(SERBIN)    
    rm -f $(CLIBIN)    
