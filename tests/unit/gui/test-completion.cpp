/*
 * test-completion.cpp - test gui completion functions
 */

#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>
#include "tests/tests.h"
#include "src/core/wee-config.h"
#include "src/core/wee-arraylist.h"
#include "src/gui/gui-buffer.h"
#include "src/gui/gui-completion.h"
#include "src/gui/gui-input.h"
#include "src/gui/gui-nicklist.h"
#include "src/plugins/plugin.h"
}

TEST_GROUP(Completion)
{
    struct t_gui_buffer *buffer;

    void setup()
    {
        buffer = gui_buffer_new (NULL, "testing_buffer",
                                 NULL, NULL, NULL,
                                 NULL, NULL, NULL);
    }

    void teardown()
    {
        gui_buffer_close(buffer);
    }
};

/*
 * Test completion of nicks with multiple options.
 *
 * Tests functions:
 *   gui_completion_buffer_init
 *   gui_completion_list_add
 *   gui_completion_search
 *   gui_completion_auto
 *   gui_completion_complete
 */

TEST(Completion, Next)
{
    /* there may be a fancy way to beforeAll / beforeEach style buildup */
    config_file_option_reset(config_completion_nick_ignore_chars, 0);

    struct t_gui_buffer *buffer;
    struct t_gui_completion *completion;

    buffer = (struct t_gui_buffer *) malloc (sizeof (struct t_gui_buffer));
    completion = (struct t_gui_completion *) malloc (sizeof (struct t_gui_completion));

    gui_completion_buffer_init (completion, buffer);

    gui_completion_list_add (completion, "Alice", 1, WEECHAT_LIST_POS_END);
    gui_completion_list_add (completion, "Alice[Ignore]", 1, WEECHAT_LIST_POS_END);
    gui_completion_list_add (completion, "Alice|Phone", 1, WEECHAT_LIST_POS_END);

    /* fake it */
    completion->context = GUI_COMPLETION_AUTO;
    completion->position = 1;

    completion->base_word = strdup ("A");

    char *str; /* for test macro */

    /* completion->position is updated in gui_input_complete which is above
     * these in the call chain, so it's not run */
    gui_completion_search (completion, 1, NULL, 1, 1);
    WEE_TEST_STR(completion->word_found, strdup ("Alice:"));

    gui_completion_search (completion, 1, NULL, 1, 1);
    WEE_TEST_STR(completion->word_found, strdup ("Alice[Ignore]:"));

    gui_completion_search (completion, 1, NULL, 1, 1);
    WEE_TEST_STR(completion->word_found, strdup ("Alice|Phone:"));

    /* test cycling */
    gui_completion_search (completion, 1, NULL, 1, 1);
    WEE_TEST_STR(completion->word_found, strdup ("Alice:"));

    gui_completion_free (completion);
    free (buffer);
}


/*
 * Test completion of nicks with multiple options.
 * Actually checks the input buffer instead of just the found return value.
 * Less hacky in that sense.
 *
 * Tests functions:
 *   gui_completion_buffer_init
 *   gui_input_complete_next
 *   gui_completion_list_add
 *   gui_completion_search
 *   gui_completion_find_context
 *   gui_completion_auto
 *   gui_completion_build_list_template
 *   gui_completion_custom
 *   gui_completion_complete
 */

TEST(Completion, NextWithBuffer)
{
    /* there may be a fancy way to beforeAll / beforeEach style buildup */
    config_file_option_reset(config_completion_nick_ignore_chars, 0);

    gui_nicklist_add_nick(buffer, buffer->nicklist_root, "Alice",
                          NULL, NULL, NULL, 1);
    gui_nicklist_add_nick(buffer, buffer->nicklist_root, "Alice[Ignore]",
                          NULL, NULL, NULL, 1);
    gui_nicklist_add_nick(buffer, buffer->nicklist_root, "Alice|Phone",
                          NULL, NULL, NULL, 1);

    gui_input_insert(buffer, "A");

    char *str; /* for test macro */

    /* completion->position is updated in gui_input_complete which is above
     * these in the call chain, so it's not run */
    gui_input_complete_next (buffer);
    WEE_TEST_STR(buffer->input_buffer, strdup ("Alice: "));

    gui_input_complete_next (buffer);
    WEE_TEST_STR(buffer->input_buffer, strdup ("Alice[Ignore]: "));

    gui_input_complete_next (buffer);
    WEE_TEST_STR(buffer->input_buffer, strdup ("Alice|Phone: "));

    /* test cycling */
    gui_input_complete_next (buffer);
    WEE_TEST_STR(buffer->input_buffer, strdup ("Alice: "));
}
