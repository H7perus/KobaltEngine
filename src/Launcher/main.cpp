#pragma once

#include <iostream>
#include "testclass.h"

#include "Engine/Entity/EntityFactory.h"

#include "Client/Client.h"

int main()
{
	KE::EntityFactory::RegisterEntity<TestEntity>("TestEntity");

	std::shared_ptr<KE::Entity> testEntityPointer = KE::EntityFactory::Create("TestEntity");


	std::cout << "the name of this class is " << testEntityPointer->GetTypeName() << std::endl;

	KE::Client TestClient; 
	TestClient.Initialise();
	TestClient.Main();

	return 1337;
}