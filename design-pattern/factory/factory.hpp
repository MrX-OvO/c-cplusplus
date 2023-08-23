//
// Created by mrxovo on 8/21/23.
//
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

class Shoes {
public:
  virtual void show() = 0;
  virtual ~Shoes() = default;
};

class NikeShoes : public Shoes {
public:
  void show() override { std::cout << "This is NikeShoes" << std::endl; }
};

class Clothes {
public:
  virtual void show() = 0;
  virtual ~Clothes() = default;
};

class UniqloClothes : public Clothes {
public:
  void show() override { std::cout << "This is UniqloClothes" << std::endl; }
};

namespace factory1 {
template <typename AbstractProduct_t> class AbstractFactory {
public:
  virtual AbstractProduct_t *CreateProduct() = 0;
  virtual ~AbstractFactory() = default;
};

template <typename AbstractProduct_t, typename ConcreteProduct_t>
class ConcreteFactory : public AbstractFactory<AbstractProduct_t> {
public:
  AbstractProduct_t *CreateProduct() override {
    return new ConcreteProduct_t();
  }
};
} // namespace factory1

namespace factory2 {
template <typename ProductType_t> class IProductRegister;

template <typename ProductType_t> class ProductFactory {
public:
  ProductFactory(const ProductFactory &) = delete;
  ProductFactory(ProductFactory &&) = delete;
  ProductFactory &operator=(const ProductFactory &) = delete;
  ProductFactory &operator=(ProductFactory &&) = delete;
  static ProductFactory *GetInstance() {
    static ProductFactory instance;
    return &instance;
  }

  void RegisterProduct(IProductRegister<ProductType_t> *reg,
                       const char *name) {
    m_ProductRegister[name] = reg;
  }

  void RegisterProduct(IProductRegister<ProductType_t> *reg,
                       const std::string &name) {
    m_ProductRegister[name] = reg;
  }

  void RegisterProduct(IProductRegister<ProductType_t> *reg,
                       std::string_view name) {
    m_ProductRegister[name] = reg;
  }

  ProductType_t *GetProduct(const char *name) {
    if (m_ProductRegister.find(name) != m_ProductRegister.end()) {
      return m_ProductRegister[name]->CreateProduct();
    }
    std::cout << "No product found for" << name << std::endl;

    return nullptr;
  }

  ProductType_t *GetProduct(const std::string &name) {
    if (m_ProductRegister.find(name) != m_ProductRegister.end()) {
      return m_ProductRegister[name]->CreateProduct();
    }
    std::cout << "No product found for" << name << std::endl;

    return nullptr;
  }

  ProductType_t *GetProduct(std::string_view name) {
    if (m_ProductRegister.find(name) != m_ProductRegister.end()) {
      return m_ProductRegister[name]->CreateProduct();
    }
    std::cout << "No product found for " << name << std::endl;

    return nullptr;
  }

protected:
  ProductFactory() = default;
  virtual ~ProductFactory() = default;

private:
  std::unordered_map<std::string, IProductRegister<ProductType_t> *>
      m_ProductRegister;
};

template <typename ProductType_t> class IProductRegister {
public:
  virtual ProductType_t *CreateProduct() = 0;
  virtual ~IProductRegister() = default;
};

template <typename ProductType_t, typename ProductImpl_t>
class ProductRegister : public IProductRegister<ProductType_t> {
public:
  explicit ProductRegister(const char *name) {
    ProductFactory<ProductType_t>::GetInstance()->RegisterProduct(this, name);
  }

  explicit ProductRegister(const std::string &name) {
    ProductFactory<ProductType_t>::GetInstance()->RegisterProduct(this, name);
  }

  explicit ProductRegister(std::string_view name) {
    ProductFactory<ProductType_t>::GetInstance()->RegisterProduct(this, name);
  }

  ProductType_t *CreateProduct() override { return new ProductImpl_t(); }
};



} // namespace factory2
