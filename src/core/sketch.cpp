#include "sketch.hpp"

#include <algorithm>
#include <deque>

void Sketch::appendCom(Component_p com)
{
	this->coms.push_back(com);
}

void Sketch::removeCom(Component_p rm)
{
	this->coms.erase(remove_if(this->coms.begin(), this->coms.end(), [&](Component_p com) -> bool { return com == rm; }), this->coms.end());
}

void Sketch::upInterface()
{
	this->int_ins.clear();
	this->int_outs.clear();
	for (Component_p com : this->coms)
	{
		vector<PortIn_p> int_ins = com->getIntIns();
		this->int_ins.insert(this->int_ins.end(), int_ins.begin(), int_ins.end());

		vector<PortOut_p> int_outs = com->getIntOuts();
		this->int_outs.insert(this->int_outs.end(), int_outs.begin(), int_outs.end());
	}
}

void Sketch::onSimStart()
{
	for (Component_p com : this->coms)
	{
		com->onSimStart();
	}
}

void Sketch::onChangeTime(double dt)
{
	/*
	chcoms.insert のときに重複を消すべき -> 未実装
	*/
	deque<Component_p> chcoms;

	for (Component_p com : this->coms)
	{
		deque<Component_p> part_chcoms;
		part_chcoms = com->onChangeTime(dt);
		chcoms.insert(chcoms.end(), part_chcoms.begin(), part_chcoms.end());
	}

	while (!chcoms.empty())
	{
		Component_p com = chcoms.front();
		chcoms.pop_front();

		deque<Component_p> part_chcoms = com->onChangeIn();
		chcoms.insert(chcoms.end(), part_chcoms.begin(), part_chcoms.end());
	}
}

void Sketch::onSimEnd()
{
	for (Component_p com : this->coms)
	{
		com->onSimEnd();
	}
}

void Sketch::exportExtends()
{
	for (Component_p com : this->coms)
	{
		com->exportExtends();
	}
}

/*
	export(){
		var ex = {};
		ex.coms = [];
		this.coms.forEach((com) => {
			ex.coms.push(com.export());
		});
		return ex;
	}

	import(im){
		this.clearCom();
		var lut = [];
		im.coms.forEach((im) => {
			var com = eval("new "+im.type+"()");
			lut = lut.concat(com.import(im));
			this.appendCom(com);
		});

		var comlut = [];
		this.coms.forEach((com) => {
			com.connectById(lut);
			comlut.push({ id: com.id, inst: com });
		});
		return comlut;
	}
};
*/