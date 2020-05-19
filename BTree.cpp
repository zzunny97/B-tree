// Please implement most of your source codes here. 
#include "BTree.h"

BTreeNode::BTreeNode(NodeType _type) {
	num_keys = 0;	
	parent = NULL;
	type = _type;
	for(int i=0; i<NUM_KEYS+1; i++) child[i] = NULL;		
}

NodeType BTreeNode::getNodeType() {
	return type;
}

string BTreeNode::getNodeType2() {
	if(type == ROOT) return "ROOT";
	else if(type == INTERNAL) return "INTERNAL";
	else if(type == LEAF) return "LEAF";
	return "ERROR";
}

bool BTreeNode::can_merge(BTreeNode* sibling) {
	if(sibling == NULL) return false;
	int sum_of_keys= num_keys + sibling->num_keys;
	if(sum_of_keys >= ceil(NUM_KEYS/2) && sum_of_keys <= NUM_KEYS) return true;
	return false;
}

bool BTreeNode::can_borrow(BTreeNode* sibling) {
	if(sibling==NULL) return false;
	if(sibling->num_keys-1 < ceil((float)(NUM_KEYS)/2)) return false;
	return true;
}

BTreeNode* BTreeNode::sibling(bool is_left) {
	if(parent == NULL) {
		cout << "[ERROR] BTreeNode::sibling(), parent not defined" << endl;
		exit(1);
	}
	int i;
	for(i=0; i<parent->num_keys+1; i++) {
		if(parent->child[i] == this) {
			break;
		}
	}
	if(i==parent->num_keys+1) {
		cout << "[ERROR] BTreeNode::sibling(), child not found" << endl;
		exit(1);
	}
	if(is_left) {
		// find left sibling
		if(i>0) return this->parent->child[i-1];
	}
	else {
		// find right sibling
		if(i<parent->num_keys) return this->parent->child[i+1];
	}
	return NULL;
}

bool BTreeNode::too_few() {
	if(type == LEAF) {
		if(num_keys < ceil((float)(NUM_KEYS)/2)) return true;
	}

	else if(type == INTERNAL) {
		int num_ptr = num_keys+1;
		if(num_ptr < ceil((float)(NUM_KEYS+1)/2)) return true;
	}
	else {
		if(num_keys < 1) return true;
	}
	return false;
}

void BTreeNode::printLeafNode() {
	for(int i=0; i<num_keys; i++) {
		cout << keys[i] << " ";
	}
	cout << endl;
}

int BTreeNode::findIndex(long long value) {
	int i;
	for(i=0; i<num_keys; i++) {
		if(value <= keys[i]) break;
	}
	return i;
}

int BTreeNode::get_child_num() {
	return num_keys+1;
}

BTreeNode* BTreeNode::find_last_non_null() {
	for(int i=num_keys; i>=0; i--) {
		if(child[i] != NULL) return child[i];
	}
	cout << "emergency! find_last_non_null is NULL" << endl;
	return NULL;
}





void BTree::print(BTreeNode* cur) {
	if(cur->type == ROOT)
		cout << "depth: " << depth << endl;
	for(int i=0; i<cur->num_keys; i++) {
		cout << cur->keys[i] << " ";
	}
	cout << endl;
	
	if(cur->type != LEAF && cur->child[0] != NULL) {
		for(int i=0; i<cur->num_keys+1; i++) {
			print(cur->child[i]);
		}
	}
}

void BTree::printAllLeafNode() {
	BTreeNode* cur = root;
	while(cur->type != LEAF) {
		if(depth==1) break;
		cur = cur->child[0];
	}
	while(cur != NULL) {
		for(int i=0; i<cur->num_keys; i++) {
			cout << cur->keys[i] << " ";
		}
		cout << " // ";
		cur = cur->child[NUM_KEYS];
	}
	cout << endl;
}


void BTree::printLeafNode(long long value) {
	BTreeNode* cur = root;
	while(cur->getNodeType() != LEAF) {
		if(depth==1) break;
		int i = cur->findIndex(value);
		if(i==cur->num_keys) {
			cur = cur->find_last_non_null();	
		}
		else if(value == cur->keys[i]) {
			cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	bool is_found = false;
	for(int i=0; i<cur->num_keys; i++) {
		if(value == cur->keys[i]) {
			is_found = true;
		} 
	}
	if(is_found) {
		for(int i=0; i<cur->num_keys; i++) {
			cout << cur->keys[i] << ", ";
		}
		cout << endl;
	}
}

void BTree::insert_in_leaf(BTreeNode* n, long long value) {
	if(n->num_keys == 0) {
		n->keys[n->num_keys++] = value;
		return;
	}
	int idx = n->findIndex(value);
	if(idx == n->num_keys) {
		n->keys[n->num_keys++] = value;
	}
	else {
		for(int i=n->num_keys-1; i>= idx; i--)
			n->keys[i+1] = n->keys[i];
		n->keys[idx] = value;
		n->num_keys++;
	}
}


void BTree::insert_in_parent(BTreeNode* n1, long long value, BTreeNode* n2) {
	if(n1->type == ROOT) {
		// create a new node r containing n1, value, n2
		// make r the root of the tree
		// make n1 to internal
		BTreeNode* newroot = new BTreeNode(ROOT);
		newroot->child[0] = n1;
		newroot->keys[0] = value;
		newroot->child[1] = n2;
		newroot->num_keys++;
		if(depth == 1) {
			n1->type = LEAF;
			n2->type = LEAF;
		} 
		else {
			n1->type = INTERNAL;
			n2->type = INTERNAL;
		}
		n1->parent = newroot;
		n2->parent = newroot;
		root = newroot;
		depth++;
		return;
	}
	BTreeNode* p = n1->parent;
	if(p->num_keys < NUM_KEYS) {
		// room available in parent
		// insert value and pointer n2 into p right after n1
		for(int i=0; i<p->num_keys+1; i++) {
			if(p->child[i] == n1) {
				for(int j=p->num_keys-1; j>=i; j--) p->keys[j+1] = p->keys[j]; // move keys backward
				for(int j=p->num_keys; j>=i+1; j--) p->child[j+1] = p->child[j]; // move childs backward
				p->keys[i] = value;
				p->child[i+1] = n2;
				p->num_keys++;
				n2->parent = p;
				break;
			}
		}
	}
	else {
		// room not available in parent
		// insert original parent keys and pointers

		vector<long long> tmp_val;
		vector<BTreeNode*> tmp_ptr;
		for(int i=0; i<p->num_keys; i++) {
			tmp_val.push_back(p->keys[i]);
		}
		for(int i=0; i<p->num_keys+1; i++) tmp_ptr.push_back(p->child[i]);

		for(int i=0; i<tmp_ptr.size(); i++) {
			if(tmp_ptr[i] == n1) {
				tmp_val.insert(tmp_val.begin() + i, value);
				tmp_ptr.insert(tmp_ptr.begin() + (i+1) ,n2);
				break;
			}
		}

		p->num_keys = 0;
		BTreeNode* newparent = new BTreeNode(INTERNAL);

		int index = ceil((float)(NUM_KEYS)/2);

		for(int i=0; i<index+1; i++) {
			p->child[i] = tmp_ptr[i];
			p->child[i]->parent = p;
		}
		for(int i=0; i<index; i++) {
			p->keys[i] = tmp_val[i];
		}
		p->num_keys = index;

		long long not_inserted_key = tmp_val[index];

		for(int i=index+1; i<tmp_ptr.size(); i++) {
			newparent->child[i-(index+1)] = tmp_ptr[i];
			newparent->child[i-(index+1)]->parent = newparent;
		}
		for(int i=index+1; i<tmp_val.size(); i++) {
			newparent->keys[i-(index+1)] = tmp_val[i];
		}
		newparent->num_keys = tmp_val.size() - (index+1);
		insert_in_parent(p, not_inserted_key, newparent);
	}
}

void BTree::insert(long long value) {
	BTreeNode* cur = root;

	while(cur->type != LEAF) {
		if(depth==1) break;
		int i = cur->findIndex(value);
		if(i==cur->num_keys) {
			cur = cur->child[cur->num_keys];
		}
		else if(value == cur->keys[i]) {
			cout << "Duplicate key" << endl;
			return;
			//cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	if(cur->num_keys < NUM_KEYS) {
		insert_in_leaf(cur, value);
	}
	else {
		// split in leaf
		BTreeNode* newleaf = new BTreeNode(LEAF);
		vector<long long> tmp;
		for(int i=0; i<cur->num_keys; i++)   tmp.push_back(cur->keys[i]);
		tmp.push_back(value);	
		sort(tmp.begin(), tmp.end());
		newleaf->child[NUM_KEYS] = cur->child[NUM_KEYS]; 
		cur->child[NUM_KEYS] = newleaf;
		cur->num_keys = 0;
		for(int i=0; i<tmp.size(); i++) {
			if(i < ceil((float)(NUM_KEYS+1)/2)) {
				cur->keys[cur->num_keys++] = tmp[i];
			} else {
				newleaf->keys[newleaf->num_keys++] = tmp[i];
			}
		}
		int smallest_key_of_newleaf = newleaf->keys[0];
		insert_in_parent(cur, smallest_key_of_newleaf, newleaf);
	}
}

void BTree::borrow(BTreeNode* left, BTreeNode* right, int position, bool n_idx) {
	if(left->parent != right->parent) {
		cout << "[ERROR]: borrow parent different" << endl;
		if(left->parent == NULL) cout << "left parent is Null" << endl;
		else cout << "left parent: "; left->parent->printLeafNode();
		if(right->parent == NULL) cout << "right parent is Null" << endl;
		else cout << "right parent: "; right->parent->printLeafNode();
		cout << "left: "; left->printLeafNode();
		cout << "rhgt: "; right->printLeafNode();
		exit(1);
	}
	if(n_idx == 0) {
		if(left->type != LEAF) {
			left->keys[left->num_keys] = left->parent->keys[position];
			right->child[0]->parent = left;
			left->child[left->num_keys+1] = right->child[0];
			left->num_keys++;
			left->parent->keys[position] = right->keys[0];

			for(int i=0; i<right->num_keys; i++) right->keys[i] = right->keys[i+1];
			for(int i=0; i<right->num_keys+1; i++) right->child[i] = right->child[i+1];
			right->num_keys--;
		}
		else {
			left->keys[left->num_keys] = right->keys[0];
			left->num_keys++;
			for(int i=0; i<right->num_keys; i++) right->keys[i] = right->keys[i+1];
			right->num_keys--;
			left->parent->keys[position] = right->keys[0];
		}
	}
	else {
		if(right->type != LEAF) {
			for(int i=right->num_keys-1; i>=0; i--) right->keys[i+1] = right->keys[i];
			for(int i=right->num_keys; i>=0; i--) right->child[i+1] = right->child[i];
			right->keys[0] = left->parent->keys[position];
			left->child[left->num_keys]->parent = right;
			right->child[0] = left->child[left->num_keys];
			right->num_keys++;
			left->parent->keys[position] = left->keys[left->num_keys-1];
			left->child[left->num_keys] = NULL;
			left->num_keys--;
		}
		else {
			for(int i=right->num_keys-1; i>=0; i--) right->keys[i+1] = right->keys[i];
			right->keys[0] = left->keys[left->num_keys-1];
			right->num_keys++;
			left->num_keys--;
			left->parent->keys[position] = right->keys[0];
		}
	}
}

void BTree::merge(BTreeNode* left, BTreeNode* right, int position) {
	if(left->parent != right->parent) {
		cout << "[ERROR]: merge parent different" << endl;
		if(left->parent == NULL) cout << "left parent is Null" << endl;
		else cout << "left parent: "; left->parent->printLeafNode();
		if(right->parent == NULL) cout << "right parent is Null" << endl;
		else cout << "right parent: "; right->parent->printLeafNode();
		cout << "left: "; left->printLeafNode();
		cout << "rhgt: "; right->printLeafNode();
		exit(1);
	}
	//cout << "MERGE: " << endl;
	//cout << "left: ";  left->printLeafNode(); cout << "left num_keys: " << left->num_keys << endl;
	//cout << "right: "; right->printLeafNode(); cout << "right num_keys: " << right->num_keys << endl;
	if(left->type != LEAF) {
		left->keys[left->num_keys++] = left->parent->keys[position-1];
	}
	for(int i=0; i<right->num_keys; i++) {
		left->keys[left->num_keys+i] = right->keys[i];
	}
	for(int i=0; i<right->num_keys+1; i++) {
		left->child[left->num_keys+i] = right->child[i];
	}
	left->num_keys += right->num_keys;
	//cout << "zzunny" << endl;
	for(int i=position-1; i<left->parent->num_keys; i++) left->parent->keys[i] = left->parent->keys[i+1];
	for(int i=position; i<left->parent->num_keys; i++) left->parent->child[i] = left->parent->child[i+1];
	//cout << "zzunny" << endl;
	left->parent->num_keys--;
	if(left->type == LEAF) {
		left->child[NUM_KEYS] = right->child[NUM_KEYS];
	}
	else {
		for(int i=0; i<left->num_keys+1; i++) {
			left->child[i]->parent = left;
		}
	}
	//cout << "zzunny" << endl;
	delete right;
}

void BTree::delete_entry(BTreeNode* n, long long value, int position) {

	BTreeNode* parent = n->parent;
	long long last_min = n->keys[0];

	/*
	while(cur->getNodeType() != LEAF) {
		if(depth==1)  break;
		int i = cur->findIndex(value);
		if(i==cur->num_keys) {
			cur = cur->find_last_non_null();
		}
		else if(value == cur->keys[i]) {
			cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	*/

	//cout << "before delete "; n->printLeafNode();
	
	if(n==NULL) return;
	if(n->type != LEAF && n->child[0] != NULL) {
		int i = n->findIndex(value);
		//cout << "index: " << i << endl;
		if(i==n->num_keys) {
			delete_entry(n->child[n->num_keys], value, n->num_keys);
		}
		else if(value == n->keys[i]) {
			delete_entry(n->child[i+1], value, i+1);
		}
		else {
			delete_entry(n->child[i], value, i);
		}
	}
	//else {
		for(int i=0; i<n->num_keys; i++) {
			if(n->keys[i] == value) {
				//cout << "delete!" << endl;
				for(int j=i; j<n->num_keys-1; j++) {
					n->keys[j] = n->keys[j+1];
				}
				n->num_keys--;
				break;
			}
		}
	//}
	//cout << "after delete "; n->printLeafNode();
	//n->printLeafNode();
	
	/*
	for(int i=0; i<n->num_keys+1; i++) {
		if(n->type != LEAF && value < n->keys[i]) {
			delete_entry(n->child[i], value, i);
			break;
		}
		else if(n->type == LEAF && n->keys[i] == value) {
			n->printLeafNode();
			for(int j=i; j<n->num_keys-1; j++) {
				n->keys[j] = n->keys[j+1];
			}
			n->num_keys--;
			break;
		}
	}
	*/
	//n->printLeafNode();

	if(n->type == ROOT) {
		if(n->num_keys == 0 && n->child[0] != NULL) { // only one remaining child
			root = n->child[0];
			n->child[0]->type = ROOT;
			n->parent = NULL;
			depth--;
			delete n;
			return;
		}
	}



	if(n->type == LEAF && n->num_keys < NUM_KEYS/2) {
		BTreeNode* left = n->sibling(true); // get left sibling
		BTreeNode* right = n->sibling(false); // get right sibling
		if(left != NULL && left->num_keys-1 >= NUM_KEYS/2) {
			// borrow from left node
			borrow(left, n, position-1, 1);
		}

		else if(right != NULL && right->num_keys-1 >= NUM_KEYS/2) {
			// borrow from right node 
			borrow(n, right, position, 0);
		}

		else if(left != NULL && n->num_keys + left->num_keys <= NUM_KEYS) {
			// merge with left
			merge(left, n, position);
		}

		else if(right != NULL && n->num_keys + right->num_keys <= NUM_KEYS) {
			// merge with right
			merge(n, right, position+1);
		}
	}

	else if(n->type == INTERNAL && n->num_keys < NUM_KEYS/2) {
		BTreeNode* left = n->sibling(true);
		BTreeNode* right = n->sibling(false);
		if(left != NULL && left->num_keys-1 >= NUM_KEYS/2) {
			borrow(left, n, position-1, 1);
		}
		else if(right != NULL && right->num_keys-1>=NUM_KEYS/2) {
			borrow(n, right, position, 0);
		}
		else if(left != NULL && left->num_keys + n->num_keys <= NUM_KEYS) {
			merge(left, n, position);

		}
		else if(right != NULL && n->num_keys + right->num_keys <= NUM_KEYS) {
			merge(n, right, position+1);
		}
	}
	else {
		//cout << "Nothing had done" << endl;
	}

	BTreeNode* cur = n->parent;
	while(cur != NULL) {
		for(int i=0; i<cur->num_keys; i++) {
			if(cur->keys[i] == last_min) {
				cur->keys[i] = n->keys[0];
			}
		}
		cur = cur->parent;
	}


}


void BTree::remove(long long value) {
	BTreeNode* cur = root;
	while(cur->getNodeType() != LEAF) {
		if(depth==1)  break;
		int i = cur->findIndex(value);
		if(i==cur->num_keys) {
			cur = cur->find_last_non_null();
		}
		else if(value == cur->keys[i]) {
			cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	//cur->printLeafNode();
	bool is_found = false;
	for(int i=0; i<cur->num_keys; i++) {
		if(value == cur->keys[i]) {
			is_found = true;
			break;
		}
	}
	if(is_found) {
		delete_entry(root, value, 0);
	}
	else {
		cout << "No such key" << endl;
		return;
	}
}

void BTree::pointQuery(long long value) {
	BTreeNode* cur = root;
	while(cur->getNodeType() != LEAF) {
		if(depth==1) break;
		int i = cur->findIndex(value);
		if(i==cur->num_keys) {
			cur = cur->find_last_non_null();	
		}
		else if(value == cur->keys[i]) {
			cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	bool is_found = false;
	for(int i=0; i<cur->num_keys; i++) {
		if(value == cur->keys[i]) {
			is_found = true;
		} 
	}
	if(is_found) cout << value << endl;
	else cout << "NOT FOUND" << endl;
}

void BTree::rangeQuery(long long low, long long high) {
	BTreeNode* cur = root;
	while(cur->getNodeType() != LEAF) {
		if(depth==1) break;
		int i = cur->findIndex(low);
		if(i==cur->num_keys) {
			cur = cur->find_last_non_null();
		}
		else if(low == cur->keys[i]) {
			cur = cur->child[i+1];
		}
		else {
			cur = cur->child[i];
		}
	}
	int i;
	for(i=0; i<cur->num_keys; i++) {
		if(cur->keys[i] >= low) break;
	}
	if(i==cur->num_keys) i = 1 + cur->num_keys;
	bool done = false;
	while(!done) {
		int n = cur->num_keys;
		if(i<n && cur->keys[i] <= high) {
			cout << cur->keys[i] << ", ";
			i++;
		}
		else if(i<n && cur->keys[i] > high) {
			done = true;
		}
		else if(i>=n && cur->child[NUM_KEYS] != NULL) {
			cur = cur->child[NUM_KEYS];
			i=0;
		}
		else {
			done = true;
		}
	}
	cout << endl;
}










