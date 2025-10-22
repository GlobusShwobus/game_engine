#pragma once
/*

1) to minimize file IO but also avoid keeping all objects in memory at all times
	use weak pointers and maps to cache objects. When an object is first requested do file IO
	and cache it. if a copy is required look from a map and return a shared pointer from a weak ptr.
	weak ptr can know when it is dangling and shared ptr can be created from a weak ptr.
	this leaves the managment semantics to shared ptr and basic bookkeeping to the map.

	ex:

	std::shared_ptr<Entity> fastLoadEntity(Entity_ID id){

	static std::unordered_map<Entity_ID, std::weap_ptr<Entity>> cache;


	auto objPtr = cache[id].lock();

	if(!objPtr){
		objPtr=loadEntity(id);//slower load from file IO
		cache[id]=objPtr;//cahce it
		}

		return objPtr;
	}
	NOTE:: don't forget to clean up expired weak ptrs

2) (NOT SURE IF USEFUL IN MY USE CASE) observer pattern where A and C both are shared_pointers to B
	but B is also aware of A and C. To achieve this correctly B should hold a list of weap_ptr
	to all objects that point to it. thus the observer pattern, B is aware of it's observers.
	could lead to interesting possibilities but atm not sure what.

*/