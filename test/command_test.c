#include "test.h"

#include "utils/io/command.h"

test()
  describe("command")
    it("should exit with status 0")
      command_t* cmd = command_init("cmd");

      command_add_arg(cmd, "/C echo Hello World!");

      int exit_status = command_run(cmd);
      assert_equal(exit_status, 0);

      command_free(cmd);
    end()

    it("should echo \"Hello World!\"")
      command_t* cmd = command_init("cmd");

      FILE* stream_out = tmpfile();

      command_set_stdout(cmd, stream_out);
      command_add_arg(cmd, "/C echo Hello World!");

      int exit_status = command_run(cmd);
      assert_equal(exit_status, 0);

      char buf[13];
      rewind(stream_out);
      fgets(buf, 13, stream_out);
      fclose(stream_out);

      assert_str_equal(buf, "Hello World!");

      command_free(cmd);
    end()

    it("should echo \"Hello World!\" from environment variable")
      command_t* cmd = command_init("cmd");

      FILE* stream_out = tmpfile();

      command_set_stdout(cmd, stream_out);
      command_add_env(cmd, "TAU_TEST_HELLO_WORLD", "Hello World!");
      command_add_arg(cmd, "/C echo %TAU_TEST_HELLO_WORLD%");

      int exit_status = command_run(cmd);
      assert_equal(exit_status, 0);

      char buf[13];
      rewind(stream_out);
      fgets(buf, 13, stream_out);
      fclose(stream_out);

      assert_str_equal(buf, "Hello World!");

      command_free(cmd);
    end()
  end()
end()
