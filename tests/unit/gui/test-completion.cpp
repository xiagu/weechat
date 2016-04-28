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
#include "src/plugins/plugin.h"
}

#define HASHTABLE_TEST_KEY      "test"
#define HASHTABLE_TEST_KEY_HASH 5849825121ULL
#define HASHTABLE_TEST_VALUE    "this is a value"

TEST_GROUP(Completion)
{
};

/*
 * Tests functions:
 *   hashtable_new
 */

TEST(Completion, New)
{
    struct t_gui_buffer *buffer;
    struct t_gui_completion *completion;

    buffer = (struct t_gui_buffer *) malloc (sizeof (struct t_gui_buffer));
    completion = (struct t_gui_completion *) malloc (sizeof (struct t_gui_completion));

    gui_completion_buffer_init(completion, buffer);

    // do some tests

    gui_completion_free (completion);
    free (buffer);
}

/*
 * Create a t_gui_completion_word containing the given word.
 * Must be freed, but gui_completion_free frees contained words if called.
 */

struct t_gui_completion_word *
new_completion_word (char * word_to_complete, int is_nick)
{
    struct t_gui_completion_word *word;
    word = (struct t_gui_completion_word *) malloc (sizeof (struct t_gui_completion_word));
    word->word = word_to_complete;
    word->nick_completion = !!is_nick; /* coerce to 0 or 1 */
    word->count = 0;
    return word;
}

/*
 * Test completion of nicks with multiple options.
 *
 * Tests functions:
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

    arraylist_add(completion->list, new_completion_word (strdup ("Alice:"), 1));
    arraylist_add(completion->list, new_completion_word (strdup ("Alice[Ignore]:"), 1));
    arraylist_add(completion->list, new_completion_word (strdup ("Alice|Phone:"), 1));

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

