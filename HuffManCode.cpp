#include<iostream>
#include <fstream>
#include <cstring>
using namespace std;

class huffMan{
    public:

    class treeNode{
        public:
            string chStr;
            int frequency;
            string code;  //encoding code
            treeNode *left; //tree left
            treeNode *right;  //tree right
            treeNode *next; //linkedlist next

        treeNode(string chStr, int frequency, string code, treeNode *left, treeNode *right, treeNode *next){
            this->chStr = chStr;
            this->frequency = frequency;
            this->code = code;
            this->left = left;
            this->right = right;
            this->next = next;
        }
        void printNode(ofstream& outFile){
          // hard part: knowing what the next node will be
          // if next is a null pointer then itll give a really weird output
          // easy part: i know how to do it :)
        	outFile<<"("<< this->chStr <<", "<< this->frequency <<", "<< this->code;
          if(this->next!=nullptr) 
            outFile<<", "<<this->next->chStr;
          else 
            outFile<<", NULL";
          if(this->left!=nullptr)
            outFile<<", "<<this->left->chStr;
          else
            outFile<<", NULL";
          if(this->right!=nullptr)
            outFile<<", "<<this->right->chStr;
          else 
            outFile<<", NULL";
          outFile<<");\n";
          }
        
        bool isLeaf(){
          //leaf nodes dont have children
          if(this->left == nullptr && this->right == nullptr)
            return true;
          return false;
        }
    };

    class linkedList{
        public:
          treeNode *listHead;
          linkedList(){
              this->listHead = new treeNode("dummy", 0, "", nullptr, nullptr, nullptr);
          }
          treeNode* findSpot(treeNode** head, treeNode* newNode){
            treeNode* spot = *head; 
            while(spot->next != nullptr){
              if(spot->next->frequency >= newNode->frequency)
                return spot;
              spot = spot->next;
            }
            return spot;
          }

          void insertOneNode(treeNode* spot, treeNode* newNode){
            newNode->next = spot->next;
            spot->next = newNode;
          }

          void printList(ofstream &outFile){
            outFile<<"Printing Linked List...\n";
            treeNode* readHead = this->listHead;
            while(readHead->next!=nullptr){
              readHead->printNode(outFile);
              readHead = readHead->next;
            }
            readHead->printNode(outFile);
          }

          bool isEmpty(){
            if(this->listHead->next == nullptr)
              return true;
            return false;
          }
    };

    class binaryTree{
      public:
      treeNode *root;

      binaryTree(){
        this->root = new treeNode("", -1, "", nullptr, nullptr, nullptr);
      }

      void preOrderTraversal(treeNode *node, ofstream &outFile){
        if(node == nullptr)
          return;
        node->printNode(outFile);
        preOrderTraversal(node->left, outFile);
        preOrderTraversal(node->right, outFile);
      }

      void inOrderTraversal(treeNode *node, ofstream &outFile){
        if(node == NULL)
              return;
        inOrderTraversal(node->left, outFile);
        node->printNode(outFile);
        inOrderTraversal(node->right, outFile);
      }

      void postOrderTraversal(treeNode *node, ofstream &outFile){
        if(node == nullptr)
          return;
        postOrderTraversal(node->left, outFile);       
        postOrderTraversal(node->right, outFile);
        node->printNode(outFile);
        }
    };     
    
    //objects fo the huffedMan
    int charCountAry[256]{};
    string chrCode[256]{};
    linkedList llist;
    binaryTree tree;

    void computeCharCounts(ifstream &inFile){
      char character;
      while(!inFile.eof()){
        inFile.get(character);
        int i = (int)character;
        this->charCountAry[i]++;
      }
    }

    void printCountAry(ofstream &outFile){
      for(int i; i < 256; i++){
        char ascii = i;
        if(this->charCountAry[i] != 0){
          outFile << "Found "<< ascii << " " << this->charCountAry[i] << " time(s)"<< endl;
        }
      }
    }

    void constructHuffManLList(ofstream &outFile){
      for(int i = 0; i < 256; i++){
        if(this->charCountAry[i]>0){
          char input = i;
          string chr(1,input);
          //new line in the file
          if(i==10)
            chr = "\\n";
          treeNode *node = new treeNode(chr, this->charCountAry[i], "", nullptr, nullptr, nullptr);
          treeNode *spot = this->llist.findSpot(&this->llist.listHead, node);
          this->llist.insertOneNode(spot, node);
          this->llist.printList(outFile);
        }
      }
    }

    void constructHuffManTree(ofstream &outFile){
      outFile << "GENERATING BINARY TREE\n";
      //populates right and left node of a root
      while(this->llist.listHead->next->next != nullptr){
        //takes first 2 nodes of linked list
        treeNode* lft = this->llist.listHead->next;
        treeNode* rht = this->llist.listHead->next->next;
        string newchrstr = lft->chStr + "," + rht->chStr;
        
         int probability = lft->frequency + rht->frequency;
        treeNode *newNode = new treeNode(newchrstr, probability, "", lft, rht, nullptr);
        

        treeNode *spot = llist.findSpot(&this->llist.listHead, newNode);
        llist.insertOneNode(spot, newNode);
        this->llist.listHead->next = this->llist.listHead->next->next->next;
        llist.printList(outFile);
      }
      //once done sets the root to the lists beginning
      this->tree.root = this->llist.listHead->next;
    }

    void constructCharCode(treeNode *root, string code){
      //recursivly calls to create a charcode
      //break statement
      if(root->isLeaf()){
        root->code = code;
        int i = root->chStr[0];

        //if index is a backslash, convert it to a new line character
        if(i == 92)
          i = 10;
        this->chrCode[i] = code;
      }
      else{
        //recursive call
        constructCharCode(root->left, code + "0");
        constructCharCode(root->right, code + "1");
      }
    }

    void encode(ifstream &inFile, ofstream &outFile, ofstream &debug){
      char chr;
      while(!inFile.eof()){
        inFile.get(chr);
        int index = (int)chr;
        string code = this->chrCode[index];
        outFile << code;
        debug << index << ": " << code << endl;
      }
    }

	void decode(ifstream& inFile, ofstream& outFile){
		treeNode* spot = this->tree.root;

		char character;
		//loops until file end
		while(!inFile.eof()){
			//character found
			if(spot->isLeaf()){
        if(spot->chStr == "\\n"){
					outFile << "\n";
				}
        else
					outFile << spot->chStr;
				spot = this->tree.root;
			}
			else{
				//tests which direction to go
				inFile.get(character);
				int oneBit = (int)character - '0';
				if(oneBit == 0){
					spot = spot->left;
				}
				else if(oneBit == 1){
					spot = spot->right;
				}
				else{
					//happens if a 1 or 0 isnt read
					cout << "Bit read:" << oneBit << endl;
					exit(0);
				}
			}
		}
	}

  void userInterface(ofstream &outFile){
    string userAnswer;
    string compressed;
    string decompressed;    

    ifstream decompressor;
    ifstream fileToCompress;
    ofstream compressor;


    cout << "Would you like to compress a file?\n";
    while(true){
      cin >> userAnswer;
      if(userAnswer == "Y"){
        cout << "What is the name of the file you want to compress?\n";
        cin >> userAnswer;

        compressed = userAnswer + "_Compressed.txt";
        decompressed = userAnswer + "_DeCompressed.txt";

        compressor.open(compressed);
        decompressor.open(decompressed);
        fileToCompress.open(userAnswer);
        this->encode(fileToCompress, compressor, outFile);
        compressor.close();
        decompressor.close();
        fileToCompress.close();
        
        compressor.open(decompressed);
        decompressor.open(compressed);
        this->decode(decompressor, compressor);
        compressor.close();
        decompressor.close();

        cout << "Would you like to compress another file?\n";
      }
      else{
        cout << "The user typed N or gave a bad input. Now exiting program\n";
        exit(0);
      }
    }
  }

};//end of huffman

int main(int argc, char* argv[]){
        huffMan user;

        ifstream nameInFile;
        ofstream DebugFile;

        char *nameDebugFile;
        //creates a compressed file
        
        nameInFile.open(argv[1], ios::app);
        nameDebugFile = strcat(argv[1],"_Debug.txt");

        DebugFile.open(nameDebugFile);
          //step 1
        user.computeCharCounts(nameInFile);
          //step 2
        user.printCountAry(DebugFile);
          //step 3
        cout << "Generating Linked List\n";
        user.constructHuffManLList(DebugFile);
        cout << "DONE!!!\n";
          //step 4
        cout << "GENERATING BINARY TREE\n";
        user.constructHuffManTree(DebugFile);
        cout << "DONE!!!\n";
          //step 5
        user.constructCharCode(user.tree.root, ""); //'' is an empty String      
          //step 6
        user.llist.printList(DebugFile);
          //step 7a
        DebugFile << "PRE ORDER TRAVERSAL\n";
        cout << "PRE ORDER TRAVERSAL\n";
        user.tree.preOrderTraversal(user.tree.root, DebugFile);
        cout << "DONE!!!\n";
          //step 7b
        cout << "IN ORDER TRAVERSAL\n";
        DebugFile << "IN ORDER TRAVERSAL\n";
        user.tree.inOrderTraversal(user.tree.root, DebugFile);
        cout << "DONE!!!\n";
          //step 7c
        cout << "POST ORDER TRAVERSAL\n";
        DebugFile << "POST ORDER TRAVERSAL\n";
        user.tree.postOrderTraversal(user.tree.root, DebugFile);
        cout << "DONE!!!\n";
        nameInFile.close();
          //step 8
        user.userInterface(DebugFile);
        //close all files
        DebugFile.close();
}