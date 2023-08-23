//
// Created by mrxovo on 8/21/23.
//
#include "./factory/factory.hpp"

void test_factory1() {
  using namespace factory1;
  ConcreteFactory<Shoes, NikeShoes> nikeFactory;
  Shoes *shoes = nikeFactory.CreateProduct();
  shoes->show();
  delete shoes;
  shoes = nullptr;

  ConcreteFactory<Clothes, UniqloClothes> UniqloFactory;
  Clothes *clothes = UniqloFactory.CreateProduct();
  clothes->show();
  delete clothes;
  clothes = nullptr;
}

void test_factory2() {
  using namespace factory2;
  ProductRegister<Shoes, NikeShoes> nikeShoes("nike");
  Shoes *pNikeShoes = ProductFactory<Shoes>::GetInstance()->GetProduct("nike");
  pNikeShoes->show();
  delete pNikeShoes;
  pNikeShoes = nullptr;

  ProductRegister<Clothes, UniqloClothes> uniqloClothes("Uniqlo");
  Clothes *pUniqloClothes =
      ProductFactory<Clothes>::GetInstance()->GetProduct("Uniqlo");
  pUniqloClothes->show();
  delete pUniqloClothes;
  pUniqloClothes = nullptr;
}

int main() {
  std::cout<<"===test_factory1===\n";
  test_factory1();
  std::cout<<"===test_factory2===\n";
  test_factory2();

  return 0;
}
