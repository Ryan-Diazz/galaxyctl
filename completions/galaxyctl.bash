# bash completion for galaxyctl

_galaxyctl() {
    local cur=${COMP_WORDS[COMP_CWORD]}
    local words

    if (( COMP_CWORD == 1 )); then
        words="recording powerlid profile battery help"
    elif (( COMP_CWORD == 3 )); then
        if [[ ${COMP_WORDS[1]} == battery && ${COMP_WORDS[2]} == limit ]]; then
            words="60 70 80 90 100"
        fi
    elif (( COMP_CWORD == 2 )); then
        case ${COMP_WORDS[1]} in
            recording) words="block unblock toggle help" ;;
            powerlid)  words="enable disable toggle help" ;;
            battery)   words="health limit help" ;;
            profile)
                # Same file profile.c validates against
                words="help"
                if [[ -r /sys/firmware/acpi/platform_profile_choices ]]; then
                    words+=" $(</sys/firmware/acpi/platform_profile_choices)"
                fi
                ;;
        esac
    fi

    COMPREPLY=($(compgen -W "$words" -- "$cur"))
}

complete -F _galaxyctl galaxyctl
