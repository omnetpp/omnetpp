# To the extent possible under law, the author(s) have dedicated all 
# copyright and related and neighboring rights to this software to the 
# public domain worldwide. This software is distributed without any warranty. 
# You should have received a copy of the CC0 Public Domain Dedication along 
# with this software. 
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 

# /etc/bash.bashrc: executed by bash(1) for interactive shells.

# System-wide bashrc file

# Check that we haven't already been sourced.
([[ -z ${CYG_SYS_BASHRC} ]] && CYG_SYS_BASHRC="1") || return

# If not running interactively, don't do anything
[[ "$-" != *i* ]] && return

# Warnings
unset _warning_found
for _warning_prefix in '' ${MINGW_PREFIX}; do
    for _warning_file in ${_warning_prefix}/etc/profile.d/*.warning{.once,}; do
        test -f "${_warning_file}" || continue
        _warning="$(command sed 's/^/\t\t/' "${_warning_file}" 2>/dev/null)"
        if test -n "${_warning}"; then
            if test -z "${_warning_found}"; then
                _warning_found='true'
                echo
            fi
            if test -t 1
                then printf "\t\e[1;33mwarning:\e[0m\n${_warning}\n\n"
                else printf "\twarning:\n${_warning}\n\n"
            fi
        fi
        [[ "${_warning_file}" = *.once ]] && rm -f "${_warning_file}"
    done
done
unset _warning_found
unset _warning_prefix
unset _warning_file
unset _warning

# If MSYS2_PS1 is set, use that as default PS1;
# if a PS1 is already set and exported, use that;
# otherwise set a default prompt
# of user@host, MSYSTEM variable, and current_directory
[[ -n "${MSYS2_PS1}" ]] && export PS1="${MSYS2_PS1}"
[[ $(declare -p PS1 2>/dev/null | cut -c 1-11) = 'declare -x ' ]] || \
  export PS1='\[\e]0;\w\a\]\n\[\e[32m\]\u@\h \[\e[35m\]$MSYSTEM\[\e[0m\] \[\e[33m\]\w\[\e[0m\]\n\$ '

##############################################################
# OMNeT++ specific changes

export OMNETPP_ROOT=$HOME
export TCL_LIBRARY=/mingw64/lib/tcl8.6
export TERM=xterm-256color

export PATH=".:${OMNETPP_ROOT}/bin:${PATH}"

# source the git completion script if exsist
if [ -f "/usr/share/bash-completion/completions/git" ] ; then
  source "/usr/share/bash-completion/completions/git"
fi
if [ -f "/usr/share/git/git-prompt.sh" ] ; then
  source "/usr/share/git/git-prompt.sh"
fi

PS1='\[\e]0;\w\a\]\n\[\e[32m\]\[\e[33m\]$(__git_ps1 "(%s) ")\w\[\e[0m\]\$ '

PRODUCT=`cat ${OMNETPP_ROOT}/Version | sed 's/omnetpp-/OMNeT++ /' | sed 's/omnest-/OMNEST /'`
echo "Welcome to $PRODUCT!"

if [ ! -f Makefile.inc ]; then
cat <<__END__

Type "./configure" and "make" to build the simulation libraries.

When done, type "omnetpp" to start the IDE.
__END__
fi
