#include <stack>
#include "BehaviourTree.hpp"

struct Door {
	int doorNumber;
};

class Building {
	private:
		std::stack<Door*> doors;
	public:
		Building (int numDoors) {initializeBuilding(numDoors);}
		const std::stack<Door*>& getDoors() const {return doors;}
	private:
		void initializeBuilding (int numDoors) {
			for (int i = 0; i < numDoors; i++)
				doors.push (new Door{numDoors - i});
		}
};

struct DataContext {  // Acts as a storage for arbitrary variables that are interpreted and altered by the nodes.
	std::stack<Door*> doors;
	Door* currentDoor;
	Door* usedDoor = nullptr;
};

class DoorAction : public BehaviourTree::Node {
	private:
		std::string name;
		int probabilityOfSuccess;
	public:
		DoorAction (const std::string newName, int prob) : name(newName), probabilityOfSuccess(prob) {}
	private:
		virtual bool run() override {
			if (std::rand() % 100 < probabilityOfSuccess) {
				std::cout << name << " succeeded." << std::endl;
				return true;
			}
			std::cout << name << " failed." << std::endl;
			return false;
		}
};

int main() {
	std::srand(std::time(nullptr));

	BehaviourTree behaviorTree;
	DataContext data;
	Building building(5);  // Building with 5 doors to get in.
	BehaviourTree::Sequence sequence[3];
	BehaviourTree::Selector selector;
	BehaviourTree::Inverter inverter[2];
	BehaviourTree::Succeeder succeeder;
	BehaviourTree::RepeatUntilFail untilFail;
	BehaviourTree::GetStack<Door> getDoorStackFromBuilding (data.doors, building.getDoors());
	BehaviourTree::PopFromStack<Door> popFromStack (data.currentDoor, data.doors);
	BehaviourTree::SetVariable<Door> setVariable (data.usedDoor, data.currentDoor);
	BehaviourTree::IsNull<Door> isNull (data.usedDoor);
	DoorAction walkToDoor ("Walk to door", 99), openDoor ("Open door", 15), unlockDoor ("Unlock door", 25), smashDoor ("Smash door", 60), walkThroughDoor ("Walk through door", 60), closeDoor ("Close door", 100);

	behaviorTree.setRootChild (&sequence[0]);
	sequence[0].addChildren ({&getDoorStackFromBuilding, &untilFail, &inverter[0]});
	untilFail.setChild (&sequence[1]);
	inverter[0].setChild (&isNull);
	sequence[1].addChildren ({&popFromStack, &inverter[1]});
	inverter[1].setChild (&sequence[2]);
	sequence[2].addChildren ({&walkToDoor, &selector, &walkThroughDoor, &succeeder, &setVariable});
	selector.addChildren ({&openDoor, &unlockDoor, &smashDoor});
	succeeder.setChild (&closeDoor);

	if (behaviorTree.run())
		std::cout << "Congratulations!  You made it into the building!" << std::endl;
	else
		std::cout << "Sorry.  You have failed to enter the building." << std::endl;
}

/*
Possible outcome:

Trying to get through door #1.
Walk to door succeeded.
Open door failed.
Unlock door failed.
Smash door succeeded.
Walk through door failed.
Trying to get through door #2.
Walk to door succeeded.
Open door succeeded.
Walk through door failed.
Trying to get through door #3.
Walk to door succeeded.
Open door failed.
Unlock door failed.
Smash door succeeded.
Walk through door succeeded.
Close door succeeded.
The door that was used to get in is door #3.
Congratulations!  You made it into the building!
*/