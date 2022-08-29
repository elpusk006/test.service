# test.service
Run a target program from service program. 

# Using
- Use visual studio 2022.
- Use code_tools library -  fixed location c:\job\code_tools.

- install
  - change folder to test_service\Release.
  - run ts_servie_of_tester.exe -i
  - edit ts_service_of_tester.xml for your target progam.
  - run services.msc
  - start ts_service_of_tester.
- uninstall
  - run services.msc
  - stop ts_service_of_tester.
  - change folder to test_service\Release.
  - run ts_servie_of_tester.exe -u

- the target program must be terminated by you.!
- You can execute 32 or 64 bit target program.




