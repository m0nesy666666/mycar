import re
import sys
import time

from scripting import initScripting


CCS_ROOT = r"D:\TI\ccs2100\ccs"
CCXML = (
    r"D:\TI\ccs2100\ccs\scripting\python\examples\debugger"
    r"\mspm0g3507\mspm0g3507.ccxml"
)


def main() -> int:
    debugger = initScripting(ccsRoot=CCS_ROOT)
    debugger.setScriptingTimeout(15_000)
    reset_asserted = False

    try:
        configured = debugger.configure(CCXML)
        print(f"Configured cores: {configured}")
        cortex = debugger.openSession(re.compile(r"CORTEX_M0P", re.IGNORECASE))
        dap = debugger.openSession(re.compile(r"CS_DAP_0", re.IGNORECASE))
        sec_ap = debugger.openSession(re.compile(r"SEC_AP", re.IGNORECASE))

        print("Checking MSPM0 device-command GEL entry points...")
        cortex.expressions.evaluate("GEL_CLI_ListCommands()")

        print("Asserting nRESET through XDS110...")
        sec_ap.expressions.evaluate('GEL_AdvancedReset("XDS System Reset (Assert)")')
        reset_asserted = True
        time.sleep(0.5)

        print("Connecting CS_DAP while nRESET is asserted...")
        dap.target.connect()
        print("CS_DAP connected under reset.")

        debugger.setScriptingTimeout(60_000)
        print("Invoking Automatic Factory Reset...")
        cortex.expressions.evaluate("GEL_CLI_AutomaticFactoryReset()")
        print("Automatic Factory Reset GEL command returned.")
        return 0
    finally:
        if reset_asserted:
            try:
                print("De-asserting nRESET through XDS110...")
                sec_ap.expressions.evaluate(
                    'GEL_AdvancedReset("XDS System Reset (De-Assert)")'
                )
            except Exception as reset_error:
                print(f"Could not de-assert nRESET: {reset_error}", file=sys.stderr)
        debugger.shutdown()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as error:
        print(f"Factory Reset failed: {error}", file=sys.stderr)
        raise
