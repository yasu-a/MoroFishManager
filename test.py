from repository.esp_command import check_output

if __name__ == '__main__':
    # print(check_output("#aa"))

    # print(check_output("#get_feed_state"))

    # print(check_output("#clear_history"))

    # print(check_output("#led_only"))
    # while check_output("#get_feed_state") == "false":
    #     pass
    print(check_output("#get_feed_history"))

    # print(check_output("#get_feed_state"))
    # print(check_output("#led_and_feed"))
    # print(check_output("#get_feed_state"))
