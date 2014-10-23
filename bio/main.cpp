#include <iostream>
#include <string>
#include <memory>

struct Node;
typedef std::shared_ptr< Node > NodePtr;

// the node which stores gene data and links to parent and childs
// FIXME: the tree is counted from upside down, the root is actually the youngest specie
// and children are parents, but I use classic binary tree, so there is some mess left
struct Node
{
	struct GeneProb
	{
		bool isCounted;
		float paa;
		float pAa;
		float pAA;

		GeneProb() : isCounted( false ), paa( 0 ), pAa( 0 ), pAA( 0 ) { }

		// FIXME: not the best idea of initialization
		void FillFromString( const std::string & buffer )
		{
			if( buffer.empty() )
				return;

			if( buffer.compare( std::string( "aa" ) ) == 0 )
				paa = 1.0;
			else if( buffer.compare( std::string( "Aa" ) ) == 0 ||
					 buffer.compare( std::string( "aA" ) ) == 0 )
				pAa = 1.0;
			else if( buffer.compare( std::string( "AA" ) ) == 0 )
				pAA = 1.0;
			else
				std::cout << "Wrong gene sequence" << std::endl;

			isCounted = true;
		}
	};

	GeneProb gene;
	NodePtr parent;
	NodePtr leftChild;
	NodePtr rightChild;

	Node( ) : gene(), parent( nullptr ), leftChild( nullptr ), rightChild( nullptr ) { }
};

// This needs to print tree in human-readable format
// level is a number of tabs for specific tree leaf
void TraverseTree( const NodePtr & node, int level = 0 )
{
	if( node->leftChild != nullptr )
		TraverseTree( node->leftChild, level + 1 );

	for( int i = 0; i < level; i++ )
		std::cout << "\t";

	if( node->gene.isCounted )
		std::cout << node->gene.paa << " " << node->gene.pAa << " " << node->gene.pAA << std::endl;
	else
		std::cout << "not counted" << std::endl;

	if( node->rightChild != nullptr )
		TraverseTree( node->rightChild, level + 1 );
}

// The main counting function: count probabilities of gene combinations
// based on parents info
void CountNode( NodePtr node )
{
	if( node->leftChild == nullptr ||
		node->rightChild == nullptr )
		std::cout << "no ancestors" << std::endl;

	float pLeftA = node->leftChild->gene.pAa * 0.5 + node->leftChild->gene.pAA;
	float pRightA = node->rightChild->gene.pAa * 0.5 + node->rightChild->gene.pAA;

	node->gene.pAA = pLeftA * pRightA;
	node->gene.pAa = pLeftA * ( 1 - pRightA ) + ( 1 - pLeftA ) * pRightA;
	node->gene.paa = ( 1 - pLeftA ) * ( 1 - pRightA );
	node->gene.isCounted = true;
}

// allows to count the whole tree
void CountTree( NodePtr node )
{
	if( !node->leftChild->gene.isCounted )
		CountTree( node->leftChild );

	if( !node->rightChild->gene.isCounted )
		CountTree( node->rightChild );

	CountNode( node );
}

// parsing incoming newick string
int ParseNewickTree( const std::string & newick_str, NodePtr root )
{
	NodePtr cur_node = root;
	std::string buffer;

	for( auto symbol : newick_str )
	{
		switch( symbol )
		{
			case '(':
			{
				if( !cur_node->leftChild )
				{
					cur_node->leftChild = std::make_shared< Node >();
					cur_node->leftChild->parent = cur_node;
					cur_node = cur_node->leftChild;
				}
				else
				{
					std::cout << "wrong data" << std::endl;
					return -1;
				}

				break;
			}
			case 'A':
			case 'a':
			{
				buffer.push_back( symbol );
				break;
			}
			case ',':
			{
				cur_node->gene.FillFromString( buffer );
				buffer.clear();
				if( cur_node->parent == nullptr )
				{
					std::cout << "wrong data" << std::endl;
					return -3;
				}
				if( cur_node->parent->rightChild == cur_node )
				{
					std::cout << "wrong data" << std::endl;
					return -2;
				}

				cur_node->parent->rightChild = std::make_shared< Node >();
				cur_node->parent->rightChild->parent = cur_node->parent;
				cur_node = cur_node->parent->rightChild;
				break;
			}
			case ')':
			{
				cur_node->gene.FillFromString( buffer );
				buffer.clear();
				cur_node = cur_node->parent;
				break;
			}
			default:
				// do nothing
				break;
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	std::string newick_str;
	std::cin >> newick_str;

	NodePtr root = std::make_shared< Node >();

	ParseNewickTree( newick_str, root );

	std::cout << "================== initial tree: =================" << std::endl;
	TraverseTree( root );

	CountTree( root );

	std::cout << "================== counted tree: =================" << std::endl;
	TraverseTree( root );
	std::cout << "AA: " << root->gene.pAA << ", Aa: " << root->gene.pAa << ", aa: " << root->gene.paa << std::endl;
	return 0;
}

