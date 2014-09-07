#include <iostream>
#include <vector>
#include <bitset>
#include <stack>
#define M 512

using namespace std;

typedef vector<int>::iterator VertexListIterator;

struct Vertex {
 int label;
 int player;
 int priority;
 Vertex( int _label, int _player, int _priority ) :
 label(_label), player(_player), priority(_priority) {} 
};

class Graph {
 private:
  bool m[M][M];
  vector<Vertex> vertex_list;
  bitset<M> active;
  stack<bitset<M> > UndoBuffer;
 public:
  void ADD_VERTEX( Vertex & );
  void ADD_VERTEX( int label, int player, int priority );
  void DEACTIVATE_VERTEX( int );
  Vertex GET_VERTEX( int ) const;
  void ADD_EDGE( int, int );
  int NUM_VERTICES() const { return (int)vertex_list.size(); };
  int NUM_ACTIVE_VERTICES() const { return (int)active.count(); }
  vector<int> GET_VERTICES();
  vector<int> GET_ACTIVE_VERTICES() const;
  vector<int> GET_IN_VERTICES( int ) const;
  vector<int> GET_OUT_VERTICES( int ) const;
  void PRINT_MATRIX() const;
  void PRINT_VERTICES() const;
  void DEACTIVATE_VERTICES( bitset<M> remove_list );
  void UNDO_DEACTIVATION();
  bool CONNECTED_Q( const int, const int ) const;
  int GET_MAX_PRIORITY() const;
  vector<int> PRIORITY_LIST( int ) const;
  bitset<M> GET_ACTIVE() const;
};

bool Graph::CONNECTED_Q( const int from, const int to ) const
 { return (m[from][to] == 1); }

void Graph::ADD_VERTEX( Vertex &v )
 { ADD_VERTEX( v.label, v.player, v.priority ); }

void Graph::ADD_VERTEX( int label, int player, int priority )
{
 Vertex v = Vertex(label, player, priority);
 vertex_list.push_back(v);
 active.set(vertex_list.size()-1);
}

Vertex Graph::GET_VERTEX( int v ) const
 { return vertex_list[v]; }

vector<int> Graph::GET_VERTICES()
{
 vector<int> vertices_list;
 for( int v = 0; v < NUM_VERTICES(); ++v )
  if( active.test(v) )
   vertices_list.push_back(v);
 return vertices_list; 
}

vector<int> Graph::GET_IN_VERTICES( int v ) const
{
 vector<int> in_vertices_list;
 if( active.test(v) )
  for( int k = 0; k < NUM_VERTICES(); ++k )
   if( active.test(k) && CONNECTED_Q(k, v) )
    in_vertices_list.push_back(k);
 return in_vertices_list;
}

vector<int> Graph::GET_OUT_VERTICES( int v ) const
{
 vector<int> out_vertices_list;
 if( active.test(v) )
  for( int k = 0; k < NUM_VERTICES(); ++k )
   if( active.test(k) && CONNECTED_Q(v, k) )
    out_vertices_list.push_back(k);
 return out_vertices_list;
}

void Graph::DEACTIVATE_VERTEX( int v )
 { active.reset(v); }

void Graph::ADD_EDGE( int from, int to )
 { m[from][to] = 1; }

void Graph::DEACTIVATE_VERTICES( bitset<M> remove_list )
{
 UndoBuffer.push( active );
 for( int v = 0; v < NUM_VERTICES(); ++v )
  if( remove_list.test(v) )
   DEACTIVATE_VERTEX(v);
}

void Graph::UNDO_DEACTIVATION()
{
 active = UndoBuffer.top();
 UndoBuffer.pop(); 
}

void Graph::PRINT_MATRIX() const 
{
 for( int i = 0; i < NUM_VERTICES(); ++i, cout << endl )
 {
  for( int j = 0; j < NUM_VERTICES(); ++j )
   cout << m[i][j] << " ";
 }
}

int Graph::GET_MAX_PRIORITY() const
{
 int max_priority = -1;
 for( int v = 0; v < NUM_VERTICES(); ++v )
  if( active.test(v) &&
   GET_VERTEX(v).priority > max_priority)
    max_priority = GET_VERTEX(v).priority;
 return max_priority;
}

vector<int> Graph::PRIORITY_LIST( int priority ) const
{
 vector<int> priority_list;   
 for( int v = 0; v < NUM_VERTICES(); ++v )
  if( active.test(v) && 
   GET_VERTEX(v).priority == priority)
    priority_list.push_back(v);
 return priority_list;
}

bitset<M> Graph::GET_ACTIVE() const { return active; }
bitset<M> plain_attr( Graph &G, int player, vector<int> &B )
{
 bitset<M> A;
 for( VertexListIterator it = B.begin(); 
  it != B.end(); ++it )
   A.set(*it);
   
 bool finished = false, mark = false;
 while( !finished )
 {
  finished = true;
  for( int v = 0; v < G.NUM_VERTICES(); ++v )
  {
   if( !A.test(v) && 
    G.GET_VERTEX(v).player == player )
   {
    vector<int> in_list = G.GET_OUT_VERTICES(v);
    
    for( VertexListIterator it = in_list.begin(); 
     it != in_list.end(); ++it )
     {
        if( A.test(*it) )
        {
         A.set( v );
         finished = false;
         break;
        }
     }     
   }
   if( !A.test(v) && 
    G.GET_VERTEX(v).player == 1 - player )
   {
    mark = true;
    vector<int> in_list = G.GET_OUT_VERTICES(v);
    
    for( VertexListIterator it = in_list.begin();
     it != in_list.end(); ++it )
     {
        if( !A.test(*it) )
        {
         mark = false;
         break;
        }
     }
    if( mark )
    { 
     A.set( v );
     finished = false;
    }
   }
  }
 }
 return A;
}

vector<vector<int> > win( Graph &G )
{
 vector <vector<int> >  U(2), EMPTY(2);
 vector<int> V = G.GET_VERTICES();
 
 if( G.NUM_ACTIVE_VERTICES() == 0 ) return EMPTY;
 
 int d = G.GET_MAX_PRIORITY();
 vector<int> A = G.PRIORITY_LIST(d);
 
 int i = d % 2;
 int j = 1 - i;
 
 G.DEACTIVATE_VERTICES( plain_attr(G, i, A) );
 U = win(G);
 G.UNDO_DEACTIVATION();
 
 if( U[j].empty() )
 {
  U[i].clear();
  U[i] = V;
 }
 else
 {
  G.DEACTIVATE_VERTICES( plain_attr(G, j, U[j]) );        
  U = win(G);
  G.UNDO_DEACTIVATION();
	
  U[j].clear();
	
  vector<int> vertices = V;

  for( vector<int>::iterator 
   v = vertices.begin(); 
   v != vertices.end(); ++v )
   {
    bool found = false;
    for(vector<int>::iterator 
        it = U[i].begin(); 
        it != U[i].end(); ++it)
        if( *it == *v ) 
        { 
         found = true; 
         break;
        }
        if( !found )
         U[j].push_back( *v );
  }
 }
 return U;
}

int main( void )
{
 Graph G;
 vector <vector<int> > v;
 
 G.ADD_VERTEX( 0, 0, 2 );
 G.ADD_VERTEX( 1, 0, 3 );
 G.ADD_VERTEX( 2, 0, 2 );
 G.ADD_VERTEX( 3, 1, 3 );
 G.ADD_VERTEX( 4, 1, 4 );
 G.ADD_VERTEX( 5, 1, 1 );

 G.ADD_EDGE(0, 3);
 G.ADD_EDGE(1, 2);
 G.ADD_EDGE(1, 4);
 G.ADD_EDGE(2, 2);
 G.ADD_EDGE(2, 5);
 G.ADD_EDGE(3, 0);
 G.ADD_EDGE(3, 1);
 G.ADD_EDGE(4, 3);
 G.ADD_EDGE(4, 5);
 G.ADD_EDGE(6, 2);
 G.ADD_EDGE(5, 4);
 

 int i = G.GET_MAX_PRIORITY() % 2;
 int j = 1 - i ;

 v = win(G);
 
 cout << "Win for player " << i << ":";
 for( VertexListIterator it = v[0].begin(); 
 it != v[0].end(); ++it) cout << *it+1 << " ";
  cout << endl;
 cout << "Win for player " << j << ":";
 for( VertexListIterator it = v[1].begin(); 
		it != v[1].end(); ++it)
   cout << *it+1 << " ";
   cout << endl;
 
 return 0;    
}
