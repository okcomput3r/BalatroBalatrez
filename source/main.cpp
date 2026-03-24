// Include the most common headers from the C standard library
#include <stdio.h>
#include <unistd.h>

// Custom headers

#include <Utils/logs.h>
#include <application.h>

int main(int argc, char *argv[]) {
 
  if(APP::ConfigureApplication() == -1) 
  {  
    TRACE("Unable to Initializate Application Context");
    return 0;
  }

  if(APP::SetupScene())
  {
    TRACE("Couldn't set up the Scene");
    return 0;
  }
  
  //uint32_t ultimo_tiempo = 0;
  while(true)
  {
    
    APP::Update();

    APP::Render();



  } 

  APP::CleanApplication();

  return 0;
}
