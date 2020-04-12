#include "NewUserVector.h"

NewUserVector::NewUserVector()
{

}

NewUserVector* NewUserVector::instance = 0;
NewUserVector* NewUserVector::GetInstance() {
  if(0 == instance) {
    instance = new NewUserVector();
  }
  return instance;
}
