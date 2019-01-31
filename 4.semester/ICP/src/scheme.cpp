/**
* @file scheme.cpp
* @brief Scheme implementation
* @author Matúš Liščinský xlisci02, Marcin Vladimír xmarci10
*/
#include "scheme.h"
#include <cstdio>
#include <fstream>
#include <iostream>

int Scheme::idGenerator = 1;

/**
 * @brief Add block to scheme
 * @param b block which will be added
 */
void Scheme::addBlock(Block * b){
    this->Blocks.push_back(b);
}

/**
 * @brief Add connection to block
 * @param c connection which will be added
 */
void Scheme::addConnection(Connection * c){
    this->Connections.push_back(c);
}

/**
 * @brief Create file and save actual scheme to it
 * @param filename name of file where scheme will be saved
 */
void Scheme::saveScheme(std::string filename){
    std::string str;
    std::ofstream out (filename, std::ofstream::out);
    for(Block *b : this->Blocks){
        str.append(std::to_string(b->type).append(" "));
        str.append(std::to_string(b->position[0]).append(" "));
        str.append(std::to_string(b->position[1]));
        str.append("\n");
    }
    str.append("#\n");
    for(Connection* c : this->Connections){
        str.append(std::to_string(this->getIndexOfBlock(c->output->in_block)).append(" "));
        str.append(std::to_string(this->getIndexOfBlock(c->input->in_block)).append(" "));
        if(c->input->in_block->inPorts.size() == 1){
            str.append(std::to_string(15));
        }else if(&c->input->in_block->inPorts[0] == c->input){
            str.append(std::to_string(0));
        }else{
            str.append(std::to_string(30));
        }
        str.append("\n");
    }
    out << str;
    out.close();
}

/**
 * @brief Get index of block from vector of blocks in scheme
 * @param b id of block 'b' is wanted
 * @return return index of block or -1 if error
 */
int Scheme::getIndexOfBlock(Block * b){
    int index = 0;
    for(Block * t : this->Blocks){
        if(t == b)
            return index;
        index++;
    }
    return -1;
}

/**
 * @brief Compute transtitive closure of graph
 * @param graph is matrix of blocks connections
 * @param n number of all blocks in scheme
 * @return return 0 if ok or -1 if error
 */
int Scheme::transitiveClosure(int * graph, int n){
    int reach[n][n],i, j, k;

    for(int a = 0; a < n; a++){
        for(int b = 0; b < n; b++){
            reach[a][b] = graph[a*n+b];
        }
    } 
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                reach[i][j] = reach[i][j] || (reach[i][k] && reach[k][j]);
                if(i == j && reach[i][j] ==1 )
                    return -1;
            }
        }
    }
        return 0;
}

/**
 * @brief Check if our scheme contains cycle
 * @return return -1 if find cycle or 0 if not
 */
int Scheme::checkForCycle(){
    int n = this->Blocks.size();
    int matrix[n*n] = {0};
    for(Connection * c : this->Connections){
        int input_idx = getIndexOfBlock(c->input->in_block);
        int output_idx = getIndexOfBlock(c->output->in_block);
        matrix[output_idx * n + input_idx] = 1;
   }
    int result = transitiveClosure(matrix, n);
    return result;
}

/**
 * @brief Compute scheme
 * @param recursive if recursive is true compute all scheme and
 *        if recursive is false just one step is performs
 * @return return pointer to acctually computed block
 */
SchemeItem* Scheme::run(bool recursive){
    int cond = 1;
    for(Block *b : this->Blocks){
        for (Port &p : b->outPorts){
            if(p.m.empty())
                cond = 0;
        }
    }
    if(cond)
        return nullptr;
    int flag;
    for(Block * b : this->Blocks){
        flag = 0;
        for (Port &p : b->inPorts){
            b->Load();
            if (p.m.size() == 0){
                flag=1; break;
            }
        }
        bool emptyOutput = b->outPorts[0].m.empty();

        if(flag == 0 && emptyOutput){
            b->Execute();
            if(!recursive)
                return b->item;
        }
    }
    if(recursive)
        run(true);
    return nullptr;
}

/**
 * @brief Clear values on all blocks in scheme
 */
void Scheme::clearPorts(){
    for(Block *b : this->Blocks){
        for (Port &op : b->outPorts){
            op.m.clear();
        }
        for (Port &ip : b->inPorts){
            ip.m.clear();
        }
    }
}

/**
 * @brief Remove connection
 * @param c c is connection which will be removed
 */
void Scheme::removeConnection(Connection *c){
    std::vector<Connection *>::iterator it = find(this->Connections.begin(), this->Connections.end(), c);
    c->input->connected_to=NULL;
    if ( it != this->Connections.end() )
         this->Connections.erase(it);
}

/**
 * @brief Remove connection of given block
 * @param b block from which connections will be removed
 */
void Scheme::removeConnectionsOfBlock(Block *b){
    std::vector<Connection*> help = this->Connections;
    for(Connection *c : help){
        if(c->input->in_block == b || c->output->in_block == b)
            removeConnection(c);
    }
}

/**
 * @brief Remove block from scheme
 * @param b block which will be removed
 */
void Scheme::removeBlock(Block * b){
    removeConnectionsOfBlock(b);
    std::vector<Block *>::iterator it = find(this->Blocks.begin(), this->Blocks.end(), b);
    if ( it != this->Blocks.end() )
         this->Blocks.erase(it);
}

/**
 * @brief Remove all blocks and connections from scheme
 */
void Scheme::clearScheme(){
    std::vector<Block*> help = this->Blocks;
    for(Block *b: help){
        removeBlock(b);
    }
}
